#include <vector>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <gumbo.h>
#include "crawler.h"
#include "parallelCrawler.h"

int main( int argc, char** argv )
{
    WebCrawler crawler;
    std::string url = "https://www.kenyans.co.ke/";
    std::vector<std::string> urls = {
          "https://www.kenyans.co.ke/news","https://nation.africa/kenya", "https://www.standardmedia.co.ke/", 
            "https://www.the-star.co.ke/", "https://www.bbc.co.uk/news/topics/c40rjmqdlzzt/kenya","https://www.nation.co.ke/kenya",
            "https:bbc.com/news/world/africa", "https://www.kenyanews.go.ke/", "https://www.kenyanews.go.ke/category/news/",
            "https://www.kenyanews.go.ke/category/press-releases/", "https://www.kenyanews.go.ke/category/speeches/",
            "https://www.citizen.digital/","https://ntvkenya.co.ke/news/","https://www.kbc.co.ke/","https://www.k24tv.co.ke/",
            "https://www.k24tv.co.ke/news/", "https://www.kbc.co.ke/news/", "https://www.kbc.co.ke/news/local/","https://www.standardmedia.co.ke/ktnnews/category/1/news",
            "https://www.standardmedia.co.ke/ktnnews/category/1/news","https://www.standardmedia.co.ke/ktnnews/category/1/news",
  "https://www.google.com/search?q=kenya+news",
  "https://www.bing.com/search?q=kenya+news",
  "https://www.yahoo.com/news/kenya",
  "https://www.africanews.com/kenya/",
  "https://www.aljazeera.com/topics/country/kenya.html",
  "https://www.bbc.com/news/world-africa-kenya",
  "https://www.cnn.com/africa/kenya",
  "https://www.reuters.com/subjects/kenya",
  "https://www.nation.co.ke/",
  "https://www.standardmedia.co.ke/",
  "https://www.kbc.co.ke/",
  "https://www.k24tv.co.ke/",
  "https://www.the-star.co.ke/",
  "https://www.kenyanews.go.ke/",
  "https://www.citizen.digital/",
  "https://www.kenyans.co.ke/",
  "https://www.tuko.co.ke/",
  "https://www.sde.co.ke/",
  "https://www.hivisasa.com/",
  "https://www.kenyatoday.co.ke/",
  "https://www.kenyamedia.co.ke/",
  "https://www.kenyaplex.com/",
  "https://www.kenyabuzz.com/",
  "https://www.kenyadailymedia.com/",
  "https://www.kenyatribune.com/",
  "https://www.kenyascope.com/",
  "https://www.kenyapulse.com/",
  "https://www.kenyasun.co.ke/",
            };
    // push the urls to the queue
    for (auto& url : urls) {
        crawler.linkQueue.push(url);
        std::cout << "Pushed: " << url << std::endl;
    }
    std::vector<std::string> keywords = {"Spotlight", "war",};
    int numOfInstances = 2;
    if (argc > 1) {
        numOfInstances = std::stoi(argv[1]);
    }

    crawler.crawlDepth(url, numOfInstances, keywords);

    return 0;
}