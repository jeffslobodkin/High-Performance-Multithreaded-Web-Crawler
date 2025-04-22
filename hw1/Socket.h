// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#pragma once
#ifndef SOCKET_H
#define SOCKET_H

#define INITIAL_BUF_SIZE 256

class Socket {

	private:
		SOCKET sock; // socket handle
		char* buf; // current buffer
		int allocatedSize; // bytes allocated for buf
		int curPos; // current position in buffer
		struct sockaddr_in server;
		WSADATA wsaData;

	public:
		Socket();
		~Socket();
		bool Read(std::string typePage);
		void Connect(std::string request, std::string host, std::string type);
		SOCKET getSock();
		void setServer(std::string host, std::string port, bool isSecond);
		void setServer(std::string host, std::string port);
		sockaddr_in getServer();
		char* getBuf();
		void closeSocket();
};


#endif SOCKET_H