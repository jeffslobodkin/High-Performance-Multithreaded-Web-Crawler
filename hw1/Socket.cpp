// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include"pch.h"

Socket::Socket() {
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		//printf("WSAStartup failed: %d\n", iResult);
		throw("WSAStartup failed: %d\n", iResult);
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		//printf("socket() error %d\n", WSAGetLastError());
		throw("socket() error %d\n", WSAGetLastError());
	}


	buf = new char[INITIAL_BUF_SIZE];
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
	server = {0};
}

void Socket::setServer(std::string host, std::string port, bool isSecond) {
	unsigned long hostAddr;

	if (inet_addr(host.c_str()) == INADDR_NONE) {
		struct hostent* remote = gethostbyname(host.c_str());

		if (remote != NULL) {
			hostAddr = *(u_long*)remote->h_addr_list[0];
			server.sin_addr.s_addr = hostAddr;
		}
		else {
			//std::cout << "failed with " << h_errno << std::endl;
			throw("failed with %d", h_errno);
		}
	}
	else {
		hostAddr = inet_addr(host.c_str());
		server.sin_addr.s_addr = hostAddr;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons((short)std::stoi(port));
}

void Socket::setServer(std::string host, std::string port) {
	//std::cout << "\t" << "Doing DNS... ";
	auto startDNS = std::chrono::high_resolution_clock::now();
	unsigned long hostAddr;




	if (inet_addr(host.c_str()) == INADDR_NONE) {
		struct hostent* remote = gethostbyname(host.c_str());

		if (remote != NULL) {
			hostAddr = *(u_long*)remote->h_addr_list[0];

			auto endDNS = std::chrono::high_resolution_clock::now();
			auto durationDNS = std::chrono::duration_cast<std::chrono::milliseconds>(endDNS - startDNS);
			server.sin_addr.s_addr = hostAddr;
			//std::cout << "done in " << durationDNS.count() << " ms, found " << inet_ntoa(server.sin_addr) << std::endl;

		}
		else {
			//std::cout << "failed with " << h_errno << std::endl;
			throw("failed with %d", h_errno);
		}
	}
	else {
		auto endDNS = std::chrono::high_resolution_clock::now();
		auto durationDNS = std::chrono::duration_cast<std::chrono::milliseconds>(endDNS - startDNS);
		hostAddr = inet_addr(host.c_str());
		server.sin_addr.s_addr = hostAddr;
		//std::cout << "done in " << durationDNS.count() << " ms, found " << inet_ntoa(server.sin_addr) << std::endl;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons((short)std::stoi(port));
}

bool Socket::Read(std::string typePage) {
	auto startLoad = std::chrono::high_resolution_clock::now();
	//std::cout << "\t" << "Loading... ";
	fd_set readfds;
	timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	int ret;
	int THRESHOLD = 1024;
	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		if ((ret = select(0, &readfds, NULL, NULL, &timeout)) > 0)
		{
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos, 0);
			if (curPos + bytes > 16000) {
				if (typePage == "ROBOT") {
					//std::cout << "failed with exceeding max" << std::endl;
					throw("failed with exceeding max");
				}
			}
			if (curPos + bytes > 2000000) {
				if (typePage == "PAGE") {
					//std::cout << "failed with exceeding max" << std::endl;
					throw("failed with exceeding max");
				}
			}
			if (bytes < 0){
				//printf("failed with %d on recv\n", WSAGetLastError());
				throw("failed with %d on recv\n", WSAGetLastError());
			}
			if (bytes == 0) {
				buf[curPos] = '\0';
				FD_CLR(sock, &readfds);
				auto endLoad = std::chrono::high_resolution_clock::now();
				auto durationLoad = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad);
				//std::cout << "done in " << durationLoad.count() << " ms with " << curPos << " bytes" << std::endl;

				return true;
			}
			curPos += bytes;

			if (allocatedSize - curPos < THRESHOLD) {
				allocatedSize *= 2;


				char* tempBuf = new char[allocatedSize];
				memcpy(tempBuf, buf, curPos);
				delete[] buf;

				buf = tempBuf;

			}

			if (!strchr(buf, 'HTTP')) {
				//printf("failed with non-HTTP header (does not begin with HTTP/)");
				throw("failed with non-HTTP header (does not begin with HTTP/)");
			}

			auto endLoad = std::chrono::high_resolution_clock::now();
			auto durationLoad = std::chrono::duration_cast<std::chrono::milliseconds>(endLoad - startLoad);
			if (durationLoad >= std::chrono::seconds(10)) {
				//std::cout << "failed with exceeding max" << std::endl;
				throw("failed with exceeding max");
			}

		}
		else if (ret == 0) {
			//printf("failed with timeout");
			throw("failed with timeout");
		}
		else {
			//printf("failed with %d on recv\n", WSAGetLastError());
			throw("failed with %d on recv\n", WSAGetLastError());
		}
	}
	return false;
}

void Socket::Connect(std::string request, std::string host, std::string type) {
	auto start = std::chrono::high_resolution_clock::now();

	if (type == "PAGE") {
		//std::cout << "      * Connecting on page... ";
	}
	else if (type == "ROBOT") {
		//std::cout << "\tConnecting on robots... ";
	}


	int error = 0;
	int len = sizeof(error);
	getsockopt(sock, SOL_SOCKET, SO_CONNECT_TIME, (char*)&error, &len);
	if (error == SOCKET_ERROR) {

		timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		int ret;

		unsigned long mode = 1;
		int res = ioctlsocket(sock, FIONBIO, &mode);
		if (res != NO_ERROR) {
			//printf("ioctlsocket failed with error: %ld\n", res);
			throw("ioctlsocket failed with error: %ld\n", res);
		}


		int conn = connect(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
		if (conn == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				//printf("failed with %d\n", WSAGetLastError());
				throw("failed with %d\n", WSAGetLastError());
			}
		}

		fd_set writefds;
		fd_set errorfds;
		FD_ZERO(&writefds);
		FD_ZERO(&errorfds);
		FD_SET(sock, &writefds);
		FD_SET(sock, &errorfds);
		ret = select(0, NULL, &writefds, &errorfds, &timeout);

		FD_CLR(sock, &writefds);
		FD_CLR(sock, &errorfds);

		if (ret == 0) {
			//printf("failed with %d\n", WSAETIMEDOUT);
			throw("failed with %d\n", WSAETIMEDOUT);
		}
		else if (ret == SOCKET_ERROR) {
			//printf("failed with %d\n", WSAGetLastError());
			throw("failed with %d\n", WSAGetLastError());
		}
	}

	if (type == "PAGE") {
		std::string httpRequest = "GET " + request + " HTTP/1.0\r\nUser-agent: TheSlobTamuCrawler/1.0\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

		if (send(sock, httpRequest.c_str(), httpRequest.length(), 0) == SOCKET_ERROR)
		{
			//printf("Send error: %d\n", WSAGetLastError());
			throw("\tSend error: %d\n", WSAGetLastError());
		}
	}
	else if (type == "ROBOT") {
		std::string robot = "/robots.txt";
		std::string httpRequest = "HEAD " + robot + " HTTP/1.0\r\nUser-agent: TheSlobTamuCrawler/1.0\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

		if (send(sock, httpRequest.c_str(), httpRequest.length(), 0) == SOCKET_ERROR)
		{
			//printf("\tSend error: %d\n", WSAGetLastError());
			throw("\tSend error: %d\n", WSAGetLastError());
		}
	}

	auto end = std::chrono:: high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	//std::cout << "done in " << duration.count() << " ms" << std::endl;
}


SOCKET Socket::getSock() {
	return sock;
}

sockaddr_in Socket::getServer() {
	return server;
}

char* Socket::getBuf() {
	return buf;
}


void Socket::closeSocket() {
	closesocket(sock);
}

Socket::~Socket() {
	if (buf != nullptr) {
		delete[] buf;
		buf = nullptr;
	}

	closesocket(sock);
	WSACleanup();
}
