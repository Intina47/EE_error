#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"
#include <regex>
#include <fstream>


WebCrawler::WebCrawler() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

WebCrawler::~WebCrawler() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

std::string WebCrawler::crawl(const std::string& url) {
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        return readBuffer;
    } else {
        std::cerr << "curl_easy_init() failed to initialize libcurl" << std::endl;
        return "";
    }
}

std::vector<std::string> WebCrawler::extractLinks(const std::string& html) {
    std::vector<std::string> links;
    GumboOutput* output = gumbo_parse(html.c_str());
    extractLinksRecursive(output->root, links);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}

// searchKeywords
std::vector<std::string> WebCrawler::searchKeywords(const std::string& html, const std::vector<std::string>& keywords, const std::string& source) {
    std::vector<std::string> sentences = splitIntoSentences(html);
    std::vector<std::string> results;
    for (const auto& sentence : sentences) {
        for (const auto& keyword : keywords) {
            if (containsKeyword(sentence, keyword)) {
                std::string resultWithSource = sentence + + " [Source: " + source + "]";
                results.push_back(resultWithSource);
                break;
            }
        }
    }
    return results;
}

//extractText
std::string WebCrawler::extractText(const std::string& html, const std::string& source) {
    GumboOutput* output = gumbo_parse(html.c_str());
    std::string text = extractTextRecursive(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    text += " [Source: " + source + "]";
    return text;
}

bool isValidUrl(const std::string& url) {
    std::regex urlRegex("(https?|ftp)://[\\w\\-_]+(\\.[\\w\\-_]+)+([a-zA-Z0-9\\-.,@?^=%&:/~+#]*[a-zA-Z0-9\\-@?^=%&/~+#])?");
    return std::regex_match(url, urlRegex);
}

void WebCrawler::crawlDepth(const std::string& url, int depth, const std::vector<std::string>& keywords) {
    if (depth < 0) {
        std::cerr << "Invalid depth parameter." << std::endl;
        return;
    }

    std::string html = crawl(url);
    std::string source = "Source: " + url;
    std::string text = extractText(html, source);

    // Search keywords in the current page
    std::vector<std::string> keywordResults = searchKeywords(text, keywords, source);
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

// private methods
size_t WebCrawler::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void WebCrawler::extractLinksRecursive(GumboNode* node, std::vector<std::string>& links) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        links.push_back(href->value);
    }
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        extractLinksRecursive(static_cast<GumboNode*>(children->data[i]), links);
    }
}

std::vector<std::string> WebCrawler::splitIntoSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string sentence;
    for (const auto& c : text) {
        if (c == '.' || c == '?' || c == '!') {
            sentences.push_back(sentence);
            sentence.clear();
        } else {
            sentence += c;
        }
    }
    return sentences;
}

bool WebCrawler::containsKeyword(const std::string& text, const std::string& keyword) {
    return text.find(keyword) != std::string::npos;
}

//extractTextRecursive
std::string WebCrawler::extractTextRecursive(GumboNode* node) {
    if (node->type == GUMBO_NODE_TEXT) {
        return std::string(node->v.text.text);
    } else if (node->type == GUMBO_NODE_ELEMENT &&
               node->v.element.tag != GUMBO_TAG_SCRIPT &&
               node->v.element.tag != GUMBO_TAG_STYLE) {
        std::string contents = "";
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            const std::string text = extractTextRecursive(static_cast<GumboNode*>(children->data[i]));
            if (i != 0 && !text.empty()) {
                contents.append(" ");
            }
            contents.append(text);
        }
        return contents;
    } else {
        return "";
    }
}