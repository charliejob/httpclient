#include "httpclient.h"

std::string g_TtsCacheServerUrl = "";
std::string g_TtsVoice = "";
std::string g_Tts_default_vendor = ""; //cmd_format rt_detect urtasr_[vendor]
std::string g_Asr_default_vendor = ""; //cmd_format say:unimrcp_[vendor]:voice
CCurlHttp g_cCurlHttp;

struct CurlStartup {
  CurlStartup()   { curl_global_init(CURL_GLOBAL_ALL); }
  ~CurlStartup()  { curl_global_cleanup(); }
}runCurlStartup;

// internal helpers
size_t recvCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  std::cout << "debug recvCallback\n" << std::endl;
  auto &buffer = *static_cast<std::string *> (userp);
  auto n = size * nmemb;
  return buffer.append((char*)contents, n), n;
}

std::string doPostRequest(CURL *C,
                  const std::string &url,const std::string &data) {
  std::string recvBuffer;
  curl_easy_setopt(C, CURLOPT_WRITEDATA, &recvBuffer);
                    
  curl_easy_setopt(C, CURLOPT_URL, url.c_str());
  //curl_easy_setopt(C, CURLOPT_DNS_CACHE_TIMEOUT, 0);

  curl_easy_setopt(C, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(C, CURLOPT_POSTFIELDSIZE, data.length());


  CURLcode resCurl = curl_easy_perform(C);
  if (resCurl != CURLE_OK) {
    std::cout << "Error with cURL: " << curl_easy_strerror(resCurl) << '\n'
        << "  URL: " << url << '\n';

    //goto retry_state;
  }
  std::cout << "debug recvCallback buff" << recvBuffer << "\n" << std::endl;

  return recvBuffer;
}

void CCurlHttp::CURL_deleter::operator () (CURL *C) {
  curl_easy_cleanup(C);
}


const std::string CCurlHttp::postRequest(const std::string& url,const std::string& data) {
  curl_easy_setopt(C.get(), CURLOPT_POST, true);
  return doPostRequest(C.get(), url, data);
}

std::string CCurlHttp::parseJsonRes(const std::string& resultData){
  std::string res = "";
  Json::Value root;
  Json::Reader reader;
  if(!reader.parse(resultData, root))
  {
      // "parse fail";
    res = "";
  }
  else
  {  
    if(root["fileUrl"].isString()){
      res = root["fileUrl"].asString();
    }
  }
  return res;
}

CCurlHttp::CCurlHttp()
    : C(curl_easy_init()){
  assert(C != nullptr);

  curl_easy_setopt(C.get(), CURLOPT_SSL_VERIFYPEER, false);
  curl_easy_setopt(C.get(), CURLOPT_CONNECTTIMEOUT, 1L);
  curl_easy_setopt(C.get(), CURLOPT_TIMEOUT, 1L);
  curl_easy_setopt(C.get(), CURLOPT_USERAGENT, "code of hammurabi");
  curl_easy_setopt(C.get(), CURLOPT_WRITEFUNCTION, recvCallback);

     //HTTP报文头  
  struct curl_slist* headers = NULL;
  //构建HTTP报文头  
  headers = curl_slist_append(headers, "Expect: ");  //对POST请求打开VERBOSE开关，看到发出请求头信息包含 Expect: 100-Continue, 网上查找对应说明，当POST数据大于1024字节时
  headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
  curl_easy_setopt(C.get(), CURLOPT_HTTPHEADER, headers);
  
}

