// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include "pch.h"

int main(int argc, char** argv)
{
	bool isRobot = false;
	int numThreads;
	//Check that the number of arguments passed in is correct
	if (argc < 2) {
		std::cout << "Too few arguments" << std::endl;
		exit(-1);
	}
	else if (argc == 3) {
		numThreads = atoi(argv[1]);
		isRobot = true;
	}
	else if (argc > 3) {
		std::cout << "Too many arguments" << std::endl;
		exit(-1);
	}


	if (!isRobot) {
		try {
			std::string url = argv[1];

			Socket sock = Socket();
			ParseUrl URLParser = ParseUrl(url);

			URLParser.parseURL();
			sock.setServer(URLParser.getHost(), URLParser.getPort());
			sock.Connect(URLParser.getRequest(), URLParser.getHost(), "PAGE");
			sock.Read("PAGE");

			ParseResponse ResponseParser = ParseResponse(sock.getBuf());

			int headPort = ResponseParser.verifyHeader();
			if (headPort > 199 && headPort < 300) {
				ResponseParser.parseHTML(URLParser.getOriginalUrl());
			}

			printf("----------------------------------------\n");
			std::cout << ResponseParser.getHead() << std::endl;
			exit(0);
		}
		catch (...) {
			exit(0);
		}

	}


	ParseFile FileParser = ParseFile(argv[2]);

	Crawler crawler(numThreads, FileParser);

	crawler.CreateThreads();

	crawler.Shutdown();

	exit(0);
}