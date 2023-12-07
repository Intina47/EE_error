//path: crawlDepth.cpp
#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"
#include <regex>
#include <fstream>
#include <queue>
#include "parallelCrawler.h"

void WebCrawler::crawlDepth(const std::string& url, int depth, const std::vector<std::string>& keywords) {
    if (depth < 0) {
        std::cerr << "Invalid depth parameter." << std::endl;
        return;
    }

    // assign keywords to a variable
    this->keywords = keywords;
    //queue of urls to crawl
    std::queue<std::string> linkQueue;
    //add the current url to the queue
    linkQueue.push(url);

    while(!linkQueue.empty()){
        // pop the front url from the queue
        std::string currentUrl = linkQueue.front();
        linkQueue.pop();

        std::string html = crawl(currentUrl);
        std::string source = "Source: " + currentUrl;
        std::string text = extractText(html, source);

        // Search keywords in the current page
        std::vector<std::string> keywordResults = searchHeadlines(text, source);
        if(!keywordResults.empty()) {
            std::cout << source << std::endl;
            std::cout << keywordResults.size() << std::endl;
            for (auto& result : keywordResults) {
                    std::cout << result << std::endl;
            }
        } else {
                std::cerr << "No Search results found on: " << currentUrl << std::endl;
            }

        if (depth > 0) {
        // Extract links and crawl each linked page with reduced depth
        GumboOutput* output = gumbo_parse(html.c_str());
        std::vector<std::string> links = extractLinks(output->root);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        if(!links.empty()) {
            std::cout << "Links at: " << currentUrl << std::endl;
            std::cout << links.size() << std::endl;
            for (const auto& link : links) {
                if (isValidUrl(link)) {
                    // Check if the link has already been visited
                    if (visitedUrls.find(link) == visitedUrls.end()) {
                        linkQueue.push(link);
                        visitedUrls.insert(link);
                        // write to file
                        std::ofstream myfile;
                        myfile.open ("links.txt", std::ios_base::app);
                        myfile << link << std::endl;
                        myfile.close();
                    } else {
                        std::cerr << "Link already visited: " << link << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "No links found on: " << currentUrl << std::endl;
        }

        // Check for pagination and crawl next pages
        std::vector<std::string> paginationLinks = extractPaginationLinks(html);
        if(!paginationLinks.empty()) {
            for (const auto& paginationLink : paginationLinks) {
                if (isValidUrl(paginationLink)) {
                    // Check if the link has already been visited
                    if (visitedUrls.find(paginationLink) == visitedUrls.end()) {
                        visitedUrls.insert(paginationLink);
                        linkQueue.push(paginationLink);
                    } else {
                        std::cerr << "Link already visited: " << paginationLink << std::endl;
                    }
                }
            }
        } else {
            std::cerr << "No pagination links found on: " << currentUrl << std::endl;
        }
    }
    }
}