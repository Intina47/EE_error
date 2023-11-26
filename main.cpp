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
    
    // std::string html = crawler.crawl(url);
    // std::string source = "Source: " + url;
    // std::string text = crawler.extractText(html, source);
    // std::vector<std::string> results = crawler.searchKeywords(text, keywords, source);
    
    // if(!html.empty()) {
    //     // std::vector<std::string> links = crawler.extractLinks(html);
        // if(!links.empty()) {
        //     std::cout << "Links on: " << url << std::endl;
        //     std::cout << links.size() << std::endl;
        // } else {
        //     std::cerr << "No links found on: " << url << std::endl;
        // }
    //     // if(!results.empty()) {
    //     //     std::cout << "Results on: " << url << std::endl;
    //     //     std::cout << results.size() << std::endl;
    //     //     for (auto& result : results) {
    //     //         std::cout << result << std::endl;
    //     //     }
    //     // } else {
    //     //     std::cerr << "No Search results found on: " << url << std::endl;
    //     // }
    // } else {
    //     std::cerr << "No html found on: " << url << std::endl;
    // }

    return 0;
}