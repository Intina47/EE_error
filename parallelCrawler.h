//path: parallelCrawler.cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include "crawler.h"

class ParallelCrawler {
public:
    ParallelCrawler(int numThreads);
    ~ParallelCrawler();
    void start(const std::string& root);
    void waitTillDone();

private:
        struct WorkerParams {
        int maxDepth;
        std::vector<std::string> searchString;
    };

    std::queue<std::string> urlqueue;
    std::mutex urlqueue_mutex;
    std::condition_variable urlqueue_cv;
    std::condition_variable queueNotEmpty_cv;
    std::vector<std::thread> threads;
    std::atomic<int> numActiveThreads;
    std::atomic<bool> done;
    std::atomic<int> numThreads;

    void workerThread(WorkerParams params);
    void addToQueue(const std::string& url);
    std::string removeFromQueue();
    bool queueEmpty();
    void notifyThreads();
    void notifyQueueNotEmpty();
    void notifyDone();
    void waitTillQueueNotEmpty();
    void decrementNumActiveThreads();
    void incrementNumActiveThreads();
    void incrementNumThreads();
    void decrementNumThreads();
    int getNumActiveThreads();
    int getNumThreads();
    bool isDone();
    void setDone();
    void printQueue();
};