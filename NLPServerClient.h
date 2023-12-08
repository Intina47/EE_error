#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <deque>

class NLPServerClient {
    public:
    NLPServerClient() = default;
    ~NLPServerClient() = default;
        // send headlines to NLP server
    std::string sendToNLP(const std::vector<std::string>& headlines) {
        for (const auto& headline : headlines) {
            // Check if adding the current headline exceeds the limit
            if (totalLength + headline.size() >= MAX_REQUEST_LENGTH) {
                // logger("Request length exceeded. Sending request to server." + std::to_string(totalLength));
                std::cout << "Request length exceeded. Sending request to server." << std::endl;
                break;
            }

            headlinesQueue.push_back(headline);
            totalLength += headline.size();
        }

        while (!headlinesQueue.empty()) {
            std::string headlinesString;
            std::string currentHeadline = headlinesQueue.front();
            headlinesString += currentHeadline + "\n";

            CURL* curl;
            CURLcode res;
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();

            if (curl) {
                std::string serverurl = "http://0.0.0.0:9000";
                std::string endpoint = "/?properties=%7B%22annotators%22%3A%20%22tokenize%2Cssplit%2Cpos%2Cner%2Cparse%2Csentiment%22%2C%22outputFormat%22%3A%20%22json%22%7D";
                curl_easy_setopt(curl, CURLOPT_URL, (serverurl + endpoint).c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, headlinesString.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &serverResponse);

                res = curl_easy_perform(curl);

                if (res != CURLE_OK) {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Server response: " << serverResponse << std::endl;
                }

                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();
             return serverResponse;
            headlinesQueue.pop_front();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    private:
    std::string serverResponse;
    size_t totalLength = 0;
    static constexpr size_t MAX_REQUEST_LENGTH = 100000;
    std::deque<std::string> headlinesQueue;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};