// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#pragma once
#ifndef CRAWLER_H
#define CRAWLER_H

class Crawler {
private:
	HANDLE mutex;
	HANDLE eventQuit;
	HANDLE finished;
	HANDLE* threads;
	HANDLE statsDone;
	std::queue<std::string> queue;
	std::chrono::seconds elapsed_time;
	int numThreads;
	bool isComplete;
	ParseFile& FileParser;
	long activeThreads;
	long extractedURLs;
	long passedHostUnique;
	int successfulDNS;
	int passedIPUnique;
	int passedRobots;
	int crawledURLs;
	long totalLinks;
	long totalBytes;
	int http2xx, http3xx, http4xx, http5xx, httpOther;
	std::chrono::steady_clock::time_point startTime;
	int tamuLinks;
	std::mutex hostMutex;
	std::mutex ipMutex;
	std::mutex statsMutex;


public:
	Crawler(int numThreads, ParseFile& fileParser);
	~Crawler();
	void ReadFile();
	static DWORD WINAPI Run(LPVOID lpParameter);
	void CrawlURL(std::string url);
	static DWORD WINAPI StatsThreadProc(LPVOID lpParameter);
	void CreateThreads();
	void Shutdown();
};
#endif CRAWLER_H