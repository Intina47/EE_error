//path: parallelCrawler.cpp
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include <fstream>
#include "parallelCrawler.h"

void logger(const std::string& message) {
    std::ofstream logFile;
    logFile.open("log.txt", std::ios_base::app); // append instead of overwrite
    logFile << message << std::endl;
    logFile.close();
}
// Constructor Initialization List
ParallelCrawler::ParallelCrawler(int numThreads)
    : numActiveThreads(0), done(false),numThreads(numThreads) {
    for (int i = 0; i < numThreads; i++) {
        this->threads.emplace_back(&ParallelCrawler::workerThread, this, WorkerParams{0, {" "}});
        this->incrementNumActiveThreads();
        this->incrementNumThreads();
        logger("thread " + std::to_string(i) + " created");
    }
}

// Destructor
ParallelCrawler::~ParallelCrawler() {
    this->waitTillDone();
    for (auto& thread : this->threads) {
        if(thread.joinable()) {
            logger("thread joined");
            thread.join();
        }
    }
}

// Start the crawler
void ParallelCrawler::start(const std::string& root) {
    logger("adding to queue : " + root);
    this->addToQueue(root);
    this->waitTillDone(); //TODO: remove the this->waitTillDone() call from the start function, to allow the calling code to decide when to wait for the crawling to complete. hence the main thread can continue to do other work while the crawler is running.
}

// Worker thread
void ParallelCrawler::workerThread(WorkerParams params) {
    while (!this->isDone()) {
        std::string url = this->removeFromQueue();
        if (url == "") {
            break;
        }
        WebCrawler crawler;
        crawler.crawlDepth(url, params.maxDepth, params.searchString);
        std::vector<std::string> links = crawler.extractLinks(crawler.getOutputRoot());
        std::cout << "Extracting links" << std::endl;
        for (const auto& link : links) {
            this->addToQueue(link);
            logger("adding to queue : " + link);
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
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    logger("incoming call adding to queue : " + url);

    // Check if there are no active threads
    if (this->getNumActiveThreads() == 0) {
        logger("creating a new thread : ");
        this->threads.emplace_back(&ParallelCrawler::workerThread, this, WorkerParams{0, {" "}});
        this->incrementNumActiveThreads();
        this->incrementNumThreads();
    } else {
        logger("not creating a new thread : ");
    }

    this->urlqueue.push(url);
    logger("notifying threads : ");
    this->notifyQueueNotEmpty();
    logger("notifying threads done : ");
}

// Remove URL from queue
std::string ParallelCrawler::removeFromQueue() {
    std::unique_lock<std::mutex> lock(this->urlqueue_mutex);
    this->waitTillQueueNotEmpty();
    if (this->queueEmpty()) {
        return "";
    }
    std::string url = this->urlqueue.front();
    logger("removing from queue : " + url);
    this->urlqueue.pop();
    return url;
}

// Check if queue is empty
bool ParallelCrawler::queueEmpty() {
    return this->urlqueue.empty();
}

// Notify threads
void ParallelCrawler::notifyThreads() {
    this->urlqueue_cv.notify_one();
}

// Notify queue not empty
void ParallelCrawler::notifyQueueNotEmpty() {
    logger("notifying queue not empty : ");
    this->queueNotEmpty_cv.notify_one();
}

// Notify done
void ParallelCrawler::notifyDone() {
    logger("notifying done : ");
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
    while (this->isDone()) {
        // this->urlqueue_cv.wait(lock);
        if(this->urlqueue_cv.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout) {
            std::cout << "waiting for the crawler to finish : " << std::endl;
            return;
        }
    }
}

// Decrement number of active threads
void ParallelCrawler::decrementNumActiveThreads() {
    logger("decrementing numActiveThreads : ");
    this->numActiveThreads--;
}

// Increment number of active threads if the number of active threads is less than the number of threads
void ParallelCrawler::incrementNumActiveThreads() {
    if (this->getNumActiveThreads() < this->getNumThreads()) {
        logger("incrementing numActiveThreads : ");
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
    logger("numThreads : " + std::to_string(this->numThreads));
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
