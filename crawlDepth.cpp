#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"
#include <regex>
#include <fstream>

void WebCrawler::crawlDepth(const std::string& url, int depth, const std::vector<std::string>& keywords) {
    if (depth < 0) {
        std::cerr << "Invalid depth parameter." << std::endl;
        return;
    }

    std::string html = crawl(url);
    std::string source = "Source: " + url;
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
            std::cerr << "No Search results found on: " << url << std::endl;
        }

    if (depth > 0) {
        // Extract links and crawl each linked page with reduced depth
        std::vector<std::string> links = extractLinks(html);
        if(!links.empty()) {
            std::cout << "Links at: " << url << std::endl;
            std::cout << links.size() << std::endl;
            std::vector<std::string> validLinks;
            for (const auto& link : links) {
                if (isValidUrl(link)) {
                    // Check if the link has already been visited
                    if (visitedUrls.find(link) == visitedUrls.end()) {
                        validLinks.push_back(link);
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
            for (const auto& link : validLinks) {
                crawlDepth(link, depth - 1, keywords);
            }
        } else {
            std::cerr << "No links found on: " << url << std::endl;
        }
    }
}