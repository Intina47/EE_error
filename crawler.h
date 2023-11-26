#ifndef CRAWLER_H
#define CRAWLER_H
#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>

class WebCrawler {
public:
    WebCrawler();
    ~WebCrawler();
    std::string crawl(const std::string& url);
    std::vector<std::string> extractLinks(const std::string& html);
    std::vector<std::string> searchKeywords(const std::string& html, const std::vector<std::string>& keywords, const std::string& source);
    std::string extractText(const std::string& html, const std::string& source);
private:
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    void extractLinksRecursive(GumboNode* node, std::vector<std::string>& links);
    std::vector<std::string> splitIntoSentences(const std::string& text);
    bool containsKeyword(const std::string& text, const std::string& keyword);
    std::string extractTextRecursive(GumboNode* node);
};

#endif //CRAWLER_H
