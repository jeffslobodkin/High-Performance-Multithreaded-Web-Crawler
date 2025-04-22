// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include "pch.h"
#include <fstream>

ParseFile::ParseFile(char* fileNameChar) {

	file = CreateFile(fileNameChar, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d\n", GetLastError());
		exit(-1);
	}
	std::cout << "Opened " << fileNameChar << " with size ";
	seenIPs = {};
	seenHosts = {};
}

std::string ParseFile::parseFile() {
	LARGE_INTEGER li;
	if (!GetFileSizeEx(file, &li)) {
		throw std::runtime_error("GetFileSizeEx error " + std::to_string(GetLastError()));
	}

	DWORD fileSize = static_cast<DWORD>(li.QuadPart);
	std::cout << fileSize << std::endl;

	fileBuf.resize(fileSize);
	DWORD bytesRead;
	if (!ReadFile(file, &fileBuf[0], fileSize, &bytesRead, NULL) || bytesRead != fileSize) {
		throw std::runtime_error("ReadFile failed with " + std::to_string(GetLastError()));
	}

	CloseHandle(file);
	return fileBuf;
}
bool ParseFile::CheckHostUnique(std::string host) {
	auto result = seenHosts.insert(host);
	if (result.second == true) {
		uniqueNum += 1;
		return true;
	}
	else {
		throw("failed");
	}
}


bool ParseFile::ping(const std::string& ip) {
	std::string command = "ping -n 1 " + ip;
	int result = system(command.c_str());  // Execute the ping command
	return result == 0;  // Return true if the ping was successful
}

bool ParseFile::CheckIPUnique(const char* IP) {
	// Check if the IP is unique
	auto result = seenIPs.insert(IP);

	if (result.second == true) {
		// If the IP is unique, perform the ping check
		if (ping(IP)) {
			// If the IP responds to ICMP ping, write it to a file
			std::ofstream outFile("responsive_ips.txt", std::ios::app);
			if (outFile) {
				outFile << IP << std::endl;  // Write the IP to the file
				std::cout << "Responsive IP: " << IP << " written to file." << std::endl;
			}
			else {
				std::cerr << "Error opening file for writing." << std::endl;
			}
		}
		else {
			std::cout << "IP " << IP << " is not responsive." << std::endl;
		}

		return true;
	}
	else {
		// If the IP is not unique, print a message (optional)
		std::cout << "IP " << IP << " is a duplicate and was not processed." << std::endl;
		return false;
	}
}

int ParseFile::getUniqueNum() {
	return uniqueNum;
}


ParseFile::~ParseFile() {}