// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include"pch.h"

ParseUrl::ParseUrl() {
	url = "";
	originalurl = "";
	request = "";
	port = "";
	host = "";
	query = "";
}

ParseUrl::ParseUrl(std::string url) {
	this->url = url;
	this->originalurl = url;
	request = "";
	port = "";
	host = "";
	query = "";
}

void ParseUrl::parseURL() {
	std::cout << "URL: " << url << "\n \t" << "Parsing URL... ";
	if (url.find("http") == std::string::npos) {
		std::cout << "failed with invalid scheme" << std::endl;
	}
	else {
		url = url.substr(7, url.length());
	}

	if (url.find("#") != std::string::npos) {
		url = url.substr(0, url.find("#"));
	}

	if (url.find("?") != std::string::npos) {
		query = url.substr(url.find("?"), url.length());
		url = url.substr(0, url.find("?"));
	}

	if (url.find("/") != std::string::npos) {
		request = url.substr(url.find("/"), url.length());
		url = url.substr(0, url.find("/"));

		if (query != "") {
			request = request + query;
		}
	}
	else {
		request = "/";

		if (query != "") {
			request = request + query;
		}
	}

	if (url.find(":") != std::string::npos) {
		try {
			if (std::stoi(url.substr(url.find(":") + 1, url.length())) == 0) {
				std::cout << "failed with invalid port" << std::endl;
			}
			else {
				port = url.substr(url.find(":") + 1, url.length());
				url = url.substr(0, url.find(":"));
			}
		}
		catch (...) {
			std::cout << "failed with invalid port" << std::endl;
		}
	}
	else {
		port = "80";
	}

	if (url.length() > 0) {
		host = url;
	}
	else {
		std::cout << "Invalid host." << std::endl;
	}
	std::cout << "host " << host << ", port " << port << ", request " << request << std::endl;
}

void ParseUrl::parseURL(std::string url) {
	if (url.find("\r")) {
		url = url.substr(0, url.find("\r"));
	}
	this->url = url;
	originalurl = url;
	//std::cout << "URL: " << url << "\n \t" << "Parsing URL... ";
	if (url.find("http") == std::string::npos) {
		//std::cout << "failed with invalid scheme" << std::endl;
		throw("failed with invalid scheme");
	}
	else {
		url = url.substr(7, url.length());
	}

	if (url.find("#") != std::string::npos) {
		url = url.substr(0, url.find("#"));
	}

	if (url.find("?") != std::string::npos) {
		query = url.substr(url.find("?"), url.length());
		url = url.substr(0, url.find("?"));
	}

	if (url.find("/") != std::string::npos) {
		request = url.substr(url.find("/"), url.length());
		url = url.substr(0, url.find("/"));

		if (query != "") {
			request = request + query;
		}
	}
	else {
		request = "/";

		if (query != "") {
			request = request + query;
		}
	}

	if (url.find(":") != std::string::npos) {
		try {
			if (std::stoi(url.substr(url.find(":") + 1, url.length())) == 0) {
				//std::cout << "failed with invalid port" << std::endl;
				throw("failed with invalid port");
			}
			else {
				port = url.substr(url.find(":") + 1, url.length());
				url = url.substr(0, url.find(":"));
			}
		}
		catch (...) {
			throw("failed with invalid port");
		}
	}
	else {
		port = "80";
	}

	if (url.length() > 0) {
		host = url;
	}
	else {
		//std::cout << "Invalid host." << std::endl;
		throw("Invalid host.");
	}
	//std::cout << "host " << host << ", port " << port  << std::endl;
}

bool ParseUrl::isTamuDomain() {
	if (host.length() >= 8) {
		return (host.compare(host.length() - 8, 8, "tamu.edu") == 0);
	}
	return false;
}

std::string ParseUrl::getURL() {
	return url;
}

std::string ParseUrl::getOriginalUrl() {
	return originalurl;
}

std::string ParseUrl::getRequest() {
	return request;
}

std::string ParseUrl::getPort() {
	return port;
}

std::string ParseUrl::getHost() {
	return host;
}

std::string ParseUrl::getQuery() {
	return query;
}