#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"

int main() {
    WebCrawler crawler;
    std::string url = "https://www.bbc.co.uk/news/world-60525350";
    std::vector<std::string> keywords = {"crawler", "test"};
    crawler.crawlDepth(url, 2, keywords);

    return 0;
}