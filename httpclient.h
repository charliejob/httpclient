#include "curl/curl.h"
#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <chrono>
#include <thread> // sleep
#include <jsoncpp/json/json.h>
extern std::string g_TtsCacheServerUrl;
extern std::string g_TtsVoice;

extern std::string g_Tts_default_vendor; //cmd_format rt_detect urtasr_[vendor]
extern std::string g_Asr_default_vendor; //cmd_format say:unimrcp_[vendor]:voice

class CCurlHttp
{
  struct CURL_deleter
  {
    void operator () (CURL *);
  };

  typedef std::unique_ptr<CURL, CURL_deleter> unique_curl;
private:
  unique_curl C;

public:
  CCurlHttp              ();
  std::string operator()(const std::string& url,const std::string& data){ return parseJsonRes(postRequest(url,data));}
private:
  const std::string postRequest(const std::string&,const std::string&);
  std::string parseJsonRes(const std::string&);
};
extern CCurlHttp g_cCurlHttp;

