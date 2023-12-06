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
    int depth = 2;
    if (argc > 1) {
        depth = std::stoi(argv[1]);
    }
    crawler.crawlDepth(url, depth, keywords);
    return 0;
    // run the program with the following command:
    // ./main 2
}