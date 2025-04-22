// Jeffrey Slobodkin
// CSCE 463 Fall 2024
// UIN: 532002090

#include "pch.h""

Crawler::Crawler(int numThreads, ParseFile& fileParser) : FileParser(fileParser) {
    mutex = CreateMutex(NULL, 0, NULL);
    eventQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
    finished = CreateSemaphore(NULL, 0, numThreads, NULL);
    this->numThreads = numThreads;
    threads = new HANDLE[numThreads];
    isComplete = false;
    startTime = std::chrono::steady_clock::now();
    statsDone = CreateEvent(NULL, TRUE, FALSE, NULL);
    passedHostUnique = 0;
}


void Crawler::ReadFile() {
    std::string fileBuffer = FileParser.parseFile();
    std::istringstream iss(fileBuffer);
    std::string url;

    while (std::getline(iss, url)) {
        url.erase(url.find_last_not_of(" \t\r\n") + 1);
        if (!url.empty()) {
            WaitForSingleObject(mutex, INFINITE);
            queue.push(std::move(url));
            ReleaseMutex(mutex);
            ReleaseSemaphore(finished, 1, NULL);
        }
    }

    isComplete = true;
    SetEvent(eventQuit);
}


void Crawler::CrawlURL(std::string url) {
    try {
        Socket sock;
        ParseUrl URLParser;
        ParseResponse ResponseParser;
        URLParser.parseURL(url);

        {
            std::lock_guard<std::mutex> lock(hostMutex);
            if (FileParser.CheckHostUnique(URLParser.getHost())) {
                InterlockedIncrement(&passedHostUnique);
            }
            else {
                return;
            }
        }

        sock.setServer(URLParser.getHost(), URLParser.getPort());
        successfulDNS++;

        {
            std::lock_guard<std::mutex> lock(ipMutex);
            if (FileParser.CheckIPUnique(inet_ntoa(sock.getServer().sin_addr))) {
                passedIPUnique++;
            }
            else {
                return;
            }
        }

        sock.Connect(URLParser.getRequest(), URLParser.getHost(), "ROBOT");
        sock.Read("ROBOT");
        ResponseParser = ParseResponse(sock.getBuf());
        int RobotCode = ResponseParser.verifyHeader();
        if (RobotCode > 399 && RobotCode < 500) {
            passedRobots++;
            Socket pageSock;
            pageSock.setServer(URLParser.getHost(), URLParser.getPort(), true);
            pageSock.Connect(URLParser.getRequest(), URLParser.getHost(), "PAGE");
            crawledURLs++;
            pageSock.Read("PAGE");
            ResponseParser = ParseResponse(pageSock.getBuf());
            int pageCode = ResponseParser.verifyHeader();

            {
                std::lock_guard<std::mutex> lock(statsMutex);
                if (pageCode >= 200 && pageCode < 300) http2xx++;
                else if (pageCode >= 300 && pageCode < 400) http3xx++;
                else if (pageCode >= 400 && pageCode < 500) http4xx++;
                else if (pageCode >= 500 && pageCode < 600) http5xx++;
                else httpOther++;
            }

            if (pageCode > 199 && pageCode < 300) {
                int linkCount = ResponseParser.parseHTML(URLParser.getOriginalUrl());
                int64_t bytes = ((std::string)pageSock.getBuf()).length();
                int tamuLinkCount = ResponseParser.getTamuLinks();

                {
                    std::lock_guard<std::mutex> lock(statsMutex);
                    totalLinks += linkCount;
                    totalBytes += bytes;
                    tamuLinks += tamuLinkCount;
                }
            }
        }
    }
    catch (...) {}
}

DWORD WINAPI Crawler::Run(LPVOID lpParameter) {
    Crawler* crawler = static_cast<Crawler*>(lpParameter);
    std::string url;

    while (true) {
        bool shouldExit = false;

        WaitForSingleObject(crawler->mutex, INFINITE);
        if (crawler->queue.empty()) {
            if (crawler->isComplete) {
                shouldExit = true;
            }
            else {
                ReleaseMutex(crawler->mutex);
                if (WaitForSingleObject(crawler->eventQuit, 1) == WAIT_OBJECT_0) {
                    shouldExit = true;
                }
                continue;
            }
        }

        if (!shouldExit && !crawler->queue.empty()) {
            url = std::move(crawler->queue.front());
            crawler->queue.pop();
            InterlockedIncrement(&crawler->extractedURLs);
        }

        ReleaseMutex(crawler->mutex);

        if (shouldExit) {
            break;
        }
        crawler->CrawlURL(url);
        url.clear();
    }

    InterlockedDecrement(&crawler->activeThreads);
    return 0;
}

void Crawler::CreateThreads() {
    startTime = std::chrono::steady_clock::now();

    CreateThread(NULL, 0, [](LPVOID param) -> DWORD {
        static_cast<Crawler*>(param)->ReadFile();
        return 0;
        }, this, 0, NULL);

    for (int i = 0; i < numThreads; i++) {
        threads[i] = CreateThread(NULL, 0, Run, this, 0, NULL);
        InterlockedIncrement(&activeThreads);
    }

    CreateThread(NULL, 0, StatsThreadProc, this, 0, NULL);
}

void Crawler::Shutdown() {
    while (true) {
        WaitForSingleObject(mutex, INFINITE);
        if (queue.empty() && activeThreads == 0) {
            isComplete = true;
            ReleaseMutex(mutex);
            break;
        }
        ReleaseMutex(mutex);
        Sleep(100);
    }

    SetEvent(eventQuit);

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);
    for (int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }

    WaitForSingleObject(statsDone, INFINITE);
}
DWORD WINAPI Crawler::StatsThreadProc(LPVOID lpParameter) {
    Crawler* crawler = static_cast<Crawler*>(lpParameter);
    int prevCrawled = 0;
    int64_t prevBytes = 0;
    auto prevTime = std::chrono::steady_clock::now();

    while (!crawler->isComplete || crawler->activeThreads > 0) {
        Sleep(2000);
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - crawler->startTime).count();
        int numCrawledDiffPrev = crawler->crawledURLs - prevCrawled;
        int64_t numBytesDiffPrev = crawler->totalBytes - prevBytes;

        double pps = numCrawledDiffPrev / 2.0;
        double mbps = (numBytesDiffPrev * 8.0) / 2000000;

        printf("[%3d] %3d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %dK *** crawling %.1f pps @ %.1f Mbps\n",
            elapsed, crawler->activeThreads, crawler->queue.size(), crawler->extractedURLs,
            crawler->passedHostUnique, crawler->successfulDNS, crawler->passedIPUnique,
            crawler->passedRobots, crawler->crawledURLs, crawler->totalLinks / 1000,
            pps, mbps);

        prevCrawled = crawler->crawledURLs;
        prevBytes = crawler->totalBytes;
        prevTime = now;
    }

    auto finalTime = std::chrono::steady_clock::now();
    int totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(finalTime - crawler->startTime).count();

    printf("Extracted %d URLs @ %.0f/s\n", crawler->extractedURLs, crawler->extractedURLs / static_cast<double>(totalElapsed));

    printf("Looked up %d DNS names @ %.0f/s\n", crawler->passedHostUnique, crawler->passedHostUnique / static_cast<double>(totalElapsed));

    printf("Attempted %d site robots @ %.0f/s\n", crawler->passedIPUnique, crawler->passedIPUnique / static_cast<double>(totalElapsed));

    printf("Crawled %d pages @ %.0f/s (%.2f MB)\n", crawler->crawledURLs, crawler->crawledURLs / static_cast<double>(totalElapsed), crawler->totalBytes / 1000000.0);

    printf("Parsed %d links @ %.0f/s\n", crawler->totalLinks, crawler->totalLinks / static_cast<double>(totalElapsed));

    printf("HTTP codes: 2xx = %d, 3xx = %d, 4xx = %d, 5xx = %d, other = %d\n", crawler->http2xx, crawler->http3xx, crawler->http4xx, crawler->http5xx, crawler->httpOther);

    SetEvent(crawler->statsDone);

    return 0;
}

Crawler::~Crawler() {
    CloseHandle(mutex);
    CloseHandle(eventQuit);
    CloseHandle(finished);
    CloseHandle(statsDone);
    delete[] threads;
}