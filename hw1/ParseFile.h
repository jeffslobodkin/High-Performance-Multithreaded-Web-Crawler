// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#pragma once
#ifndef PARSEFILE_H
#define PARSEFILE_H

class ParseFile {
private:
	HANDLE file;
	std::set<std::string> seenIPs;
	std::set<std::string> seenHosts;
	std::string fileBuf;
	int uniqueNum;

public:
	ParseFile(char* fileName);
	~ParseFile();
	std::string parseFile();
	bool CheckHostUnique(std::string host);
	bool CheckIPUnique(const char* IP);
	int getUniqueNum();
	bool ping(const std::string& ip);
};

#endif PARSEFILE_H