#include "curl/curl.h"
#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <chrono>
#include <thread> // sleep
#include <jsoncpp/json/json.h>

struct CurlStartup {
  CurlStartup()   { curl_global_init(CURL_GLOBAL_ALL); }
  ~CurlStartup()  { curl_global_cleanup(); }
}runCurlStartup;

size_t recvCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  //std::cout << "debug recvCallback\n" << std::endl;
  auto &buffer = *static_cast<std::string *> (userp);
  auto n = size * nmemb;
  return buffer.append((char*)contents, n), n;
}

class CCurlHttp
{
public:
  CCurlHttp(): C(curl_easy_init())
  {
    assert(C != nullptr);
    curl_easy_setopt(C.get(), CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(C.get(), CURLOPT_CONNECTTIMEOUT, 1L);
    curl_easy_setopt(C.get(), CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(C.get(), CURLOPT_USERAGENT, "code of hammurabi");
    curl_easy_setopt(C.get(), CURLOPT_WRITEFUNCTION, recvCallback);
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Expect: ");  //对POST请求打开VERBOSE开关，看到发出请求头信息包含 Expect: 100-Continue, 网上查找对应说明，当POST数据大于1024字节时
    headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
    curl_easy_setopt(C.get(), CURLOPT_HTTPHEADER, headers);
  }
  std::string operator()(const std::string& url,const std::string& data)
  {
     return _postRequest(url,data);
  }
private:
  const std::string _postRequest(const std::string& url,const std::string& data)
  {
    curl_easy_setopt(C.get(), CURLOPT_POST, true);
    std::string recvBuffer;
    curl_easy_setopt(C.get(), CURLOPT_WRITEDATA, &recvBuffer);
    curl_easy_setopt(C.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(C.get(), CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(C.get(), CURLOPT_POSTFIELDSIZE, data.length());

    CURLcode resCurl = curl_easy_perform(C.get());
    if (resCurl != CURLE_OK) {
      std::cout << "Error with cURL: " << curl_easy_strerror(resCurl) << '\n'
          << "  URL: " << url << '\n';
    }
    //std::cout << "debug recvCallback buff" << recvBuffer << "\n" << std::endl;
    return recvBuffer;
  }
  struct CURL_deleter
  {
    void operator () (CURL *C){curl_easy_cleanup(C);}
  };
  typedef std::unique_ptr<CURL, CURL_deleter> unique_curl;
  unique_curl C;
};

