#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"

int main() {
    WebCrawler crawler;
    std::string url = "https://www.kenyans.co.ke/news";
    std::vector<std::string> keywords = {"#NoExcuse", "VAWG", "Spotlight", "war",};
    crawler.crawlDepth(url, 2, keywords);

    return 0;
}