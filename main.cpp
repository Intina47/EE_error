#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"

int main(int argc, char const *argv[])
{
    WebCrawler crawler;
    std::string url = "https://www.kenyans.co.ke/news/95077-remark-our-papers-kcpe-candidate-demands-after-knec-admits-errors";
    std::vector<std::string> keywords = {"#NoExcuse", "VAWG", "Spotlight", "war",};
    int depth = 2;
    if (argc > 1) {
        depth = std::stoi(argv[1]);
    }
    crawler.crawlDepth(url, depth, keywords);
    return 0;
}

// TODO: Implement parallel crawling using threads or processes (or both) to speed up crawling
// TODO: Clean up data returned from crawler 