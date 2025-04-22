// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#pragma once
#ifndef PARSEURL_H
#define PARSEURL_H

class ParseUrl {

private:
	std::string url;
	std::string originalurl;
	std::string request;
	std::string port;
	std::string host;
	std::string query;

public:
	ParseUrl();
	ParseUrl(std::string url);
	void parseURL();
	void parseURL(std::string url);
	std::string getURL();
	std::string getOriginalUrl();
	std::string getRequest();
	std::string getPort();
	std::string getHost();
	std::string getQuery();
	bool isTamuDomain();
};


#endif PARSEURL_H