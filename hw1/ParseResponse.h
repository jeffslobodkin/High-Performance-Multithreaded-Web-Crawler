// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#pragma once
#ifndef PARSERESPONSEL_H
#define PARSERESPONSEL_H

class ParseResponse {
private:
	std::string response;
	std::string head;
	std::string html;
	int tamuLinks;

public:
	ParseResponse(char* buf);
	ParseResponse();
	int parseHeader(std::string tempHead);
	int verifyHeader();
	int parseHTML(std::string url);
	std::string getResponse();
	std::string getHead();
	std::string getHTML();
	int getTamuLinks();
};


#endif PARSERESPONSEL_H