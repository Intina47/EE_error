#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

class NLPServerClient {
    public:
    NLPServerClient() = default;
    ~NLPServerClient() = default;

    // send headlines to NLP server
    void sendToNLP(const std::vector<std::string>& headlines){
        std::string headlinesString;
        for(const auto& headline : headlines){
            headlinesString += headline + "\n";
        }

        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if(curl){
            std::string serverurl = "http://0.0.0.0:9000";
            std::string endpoint = "/?properties=%7B%22annotators%22%3A%20%22tokenize%2Cssplit%2Cpos%2Cner%2Cparse%2Csentiment%22%2C%22outputFormat%22%3A%20%22json%22%7D";
            curl_easy_setopt(curl, CURLOPT_URL, (serverurl + endpoint).c_str());
            // set the http post data
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, headlinesString.c_str());
            // set the callback function to handle the server response
            std::string serverResponse;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &serverResponse);
            // perform the request
            res = curl_easy_perform(curl);
            // check for errors
            if(res != CURLE_OK){
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cout << "Server response: " << serverResponse << std::endl;
            }
            // cleanup
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
    }

    private:
    // callback function to handle the server response
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
};