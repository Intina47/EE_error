#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"

int main() {
    WebCrawler crawler;
    std::string url = "https://apnews.com/article/italy-women-violence-demonstrations-protest-d89c05325c37ee4d80214700533761c8";
    std::vector<std::string> keywords = {"#NoExcuse", "VAWG", "Spotlight", "war",};
    crawler.crawlDepth(url, 2, keywords);

    return 0;
}