#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"
#include "parallelCrawler.h"

int main( int argc, char** argv )
{
    WebCrawler crawler;
    std::string url = "https://en.wikipedia.org/wiki/Elon_Musk";
    std::vector<std::string> keywords = {"#NoExcuse", "VAWG", "Spotlight", "war",};
    int numOfInstances = 2;
    if (argc > 1) {
        numOfInstances = std::stoi(argv[1]);
    }

    crawler.crawlDepth(url, 2, keywords);
    // std::cout << "creating an instance of parrallecrawler : " << std::endl;
    // // create an instance of parrallecrawler
    // ParallelCrawler parallelCrawler(numOfInstances);
    // std::cout << "starting the parallel crawler : " << std::endl;
    // std::thread parallelCrawlerThread([&]() {
    //     parallelCrawler.start(url);
    // });
    // parallelCrawler.waitTillDone();
    // std::cout << "joining the parallel crawler thread : " << std::endl;
    // parallelCrawlerThread.join();
    // std::cout << "joining the parallel crawler thread : " << std::endl;
    // return 0;

}