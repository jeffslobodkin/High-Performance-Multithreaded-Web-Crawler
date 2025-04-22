# High-Performance Multithreaded Web Crawler

A low-level, high-throughput web crawler written in C++ using raw sockets, multithreading, and custom HTTP handling. Built to scale across thousands of threads, with deep systems-level integration via WinSock and Windows threading primitives.

## Academic Context

This project was developed as part of CSCE 463 (Networks and Distributed Processing) at Texas A&M University under Professor Dmitri Loguinov. It is shared for educational and demonstration purposes. Please respect academic integrity if currently enrolled in the course.

**Course Details:**
- Course: CSCE 463 - Networks and Distributed Processing
- Reference Text: "Computer Networking: A Top-Down Approach" (6th Ed.)
- Implementation Focus: Application-Layer Crawling, Multithreading, DNS & HTTP

## Technical Features

- **Multithreaded Crawler Core**:
  - Scales to thousands of threads
  - Uses a shared URL queue and producer-consumer pattern
  - Dedicated statistics thread for 2-second interval reporting

- **Custom Socket Handling**:
  - Raw socket creation and manual DNS resolution via WinSock
  - Full HTTP/1.0 request pipeline and robots.txt handling
  - Timeout management, custom header parsing, and HTML response parsing

- **Page and Link Parsing**:
  - HTMLParserBase to extract anchor tags
  - Thread-local parsing to eliminate synchronization bottlenecks
  - Counts external links and detects `.tamu.edu` domain references

- **Statistical Output**:
  - Tracks Q, E, H, D, I, R, C, L metrics in real-time
  - Final throughput and HTTP status breakdown summary

## Core Components

- HTTP GET/HEAD implementation using WinSock
- Producer-consumer shared queue
- Host/IP uniqueness validation
- Real-time crawler speed (pps, Mbps)
- Domain-specific link classification

## Technologies

- C++
- WinSock API
- Windows Threading (CreateThread, Critical Sections, Interlocked Ops)
- HTMLParserBase (course-provided parser)

## Building and Usage

```bash
# Build in Visual Studio 2022 using x64 Release configuration
# Run the crawler with:
crawler.exe <threads> <input-url-file>
# Example:
crawler.exe 5000 URL-input-1M.txt
