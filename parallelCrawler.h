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
    void start(const std::string& root, int maxDepth, const std::string& searchString);
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
    void waitTillDone();
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

// Constructor Initialization List
ParallelCrawler::ParallelCrawler(int numThreads)
    : numThreads(numThreads), numActiveThreads(0), done(false) {
    for (int i = 0; i < numThreads; i++) {
        this->threads.emplace_back(&ParallelCrawler::workerThread, this, WorkerParams{0, {" "}});
        this->incrementNumActiveThreads();
        this->incrementNumThreads();
    }
}

// Destructor
ParallelCrawler::~ParallelCrawler() {
    this->waitTillDone();
}

// Start the crawler
void ParallelCrawler::start(const std::string& root, int maxDepth, const std::string& searchString) {
    this->addToQueue(root);
    this->notifyQueueNotEmpty();
    this->waitTillDone();
}

// Worker thread
void ParallelCrawler::workerThread(WorkerParams params) {
    while (true) {
        std::string url = this->removeFromQueue();
        if (url == "") {
            break;
        }
        WebCrawler crawler;
        crawler.crawlDepth(url, params.maxDepth, params.searchString);
        std::vector<std::string> links = crawler.extractLinks(crawler.getOutputRoot());
        for (const auto& link : links) {
            this->addToQueue(link);
        }
    }
    this->decrementNumActiveThreads();
    if (this->getNumActiveThreads() == 0) {
        this->setDone();
        this->notifyThreads();
    }
}

// Add URL to queue
void ParallelCrawler::addToQueue(const std::string& url) {
    std::lock_guard<std::mutex> lock(this->urlqueue_mutex);
    this->urlqueue.push(url);
    this->notifyQueueNotEmpty();
}

// Remove URL from queue
std::string ParallelCrawler::removeFromQueue() {
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    this->waitTillQueueNotEmpty();
    if (this->queueEmpty()) {
        return "";
    }
    std::string url = this->urlqueue.front();
    this->urlqueue.pop();
    return url;
}

// Check if queue is empty
bool ParallelCrawler::queueEmpty() {
    return this->urlqueue.empty();
}

// Notify threads
void ParallelCrawler::notifyThreads() {
    this->urlqueue_cv.notify_all();
}

// Notify queue not empty
void ParallelCrawler::notifyQueueNotEmpty() {
    this->queueNotEmpty_cv.notify_all();
}

// Notify done
void ParallelCrawler::notifyDone() {
    this->urlqueue_cv.notify_all();
}

// Wait till queue is not empty
void ParallelCrawler::waitTillQueueNotEmpty() {
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    while (this->queueEmpty() && !this->isDone()) {
        this->queueNotEmpty_cv.wait(lock);
    }
}

// Wait till done
void ParallelCrawler::waitTillDone() {
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    while (!this->isDone()) {
        this->urlqueue_cv.wait(lock);
    }
}

// Decrement number of active threads
void ParallelCrawler::decrementNumActiveThreads() {
    this->numActiveThreads--;
}

// Increment number of active threads if the number of active threads is less than the number of threads
void ParallelCrawler::incrementNumActiveThreads() {
    if (this->getNumActiveThreads() < this->getNumThreads()) {
        this->numActiveThreads++;
    }
}

// Increment number of threads
void ParallelCrawler::incrementNumThreads() {
    this->numThreads++;
}

// Decrement number of threads
void ParallelCrawler::decrementNumThreads() {
    this->numThreads--;
}

// Get number of active threads
int ParallelCrawler::getNumActiveThreads() {
    return this->numActiveThreads;
}

// Get number of threads
int ParallelCrawler::getNumThreads() {
    return this->numThreads;
}

// Check if done
bool ParallelCrawler::isDone() {
    return this->done;
}

// Set done
void ParallelCrawler::setDone() {
    this->done = true;
}

// Print queue
void ParallelCrawler::printQueue() {
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    std::queue<std::string> temp = this->urlqueue;
    while (!temp.empty()) {
        std::cout << temp.front() << std::endl;
        temp.pop();
    }
}
