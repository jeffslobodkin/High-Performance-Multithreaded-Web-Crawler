// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include"pch.h"

ParseResponse::ParseResponse(char* response) {
	this->response = response;
	head = "";
	html = "";
	tamuLinks = 0;
}

ParseResponse::ParseResponse() {
	response = "";
	head = "";
	html = "";
	tamuLinks = 0;
}

int ParseResponse::parseHeader(std::string tempHead) {
	int status;

	tempHead = tempHead.substr(tempHead.find(" "), tempHead.length());

	status = std::stoi(tempHead.substr(1, tempHead.find(" ") - 2));

	return status;
}

int ParseResponse::verifyHeader() {
	try {
		auto startVerifHeader = std::chrono::high_resolution_clock::now();
		//std::cout << "\t" << "Verifying header... ";

		int headStatus;

		head = response.substr(0, response.find("\r\n\r\n"));
		headStatus = parseHeader(head);

		auto endVerifHeader = std::chrono::high_resolution_clock::now();
		auto durationVerifHeader = std::chrono::duration_cast<std::chrono::milliseconds>(endVerifHeader - startVerifHeader);

		//std::cout << "status code " << headStatus << std::endl;

		return headStatus;
	}
	catch (...) {
		printf("\n");
		return 0;
	}

}

int ParseResponse::parseHTML(std::string url) {
	auto startParse = std::chrono::high_resolution_clock::now();
	//std::cout << "      + Parsing page... ";

	html = response.substr(response.find("\r\n\r\n"), response.length());

	HTMLParserBase* htmlParser = new HTMLParserBase;

	char* htmlParse = &html[0];
	char* urlParse = &url[0];

	int nLinks = 0;
	char* buffer = htmlParser->Parse(htmlParse, (int)strlen(htmlParse), urlParse, url.length(), &nLinks);

	// Iterate through the links
	char* currentLink = buffer;
	for (int i = 0; i < nLinks; i++) {
		ParseUrl linkUrl;
		try {
			linkUrl.parseURL(currentLink);
			if (linkUrl.isTamuDomain()) {
				tamuLinks++;
			}
		}
		catch (...) {
		}
		currentLink += strlen(currentLink) + 1;
	}

	auto endParse = std::chrono::high_resolution_clock::now();
	auto durationParse = std::chrono::duration_cast<std::chrono::milliseconds>(endParse - startParse);
	//std::cout << "done in " << durationParse.count() << " ms" << " with " << nLinks << " links" << std::endl;

	delete htmlParser;
	return nLinks;
}

std::string ParseResponse::getResponse() {
	return response;
}

std::string ParseResponse::getHead() {
	return head;
}

std::string ParseResponse::getHTML() {
	return html;
}

int ParseResponse::getTamuLinks() {
	return tamuLinks;
}
