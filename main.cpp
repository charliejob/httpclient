#include <stdio.h>
#include "httpclient.h"
#include "threadpool.h"
#include <iostream>
int main(){
	std::string res = CCurlHttp()("localhost:9999","asd");
	std::cout << "res: " << res << std::endl; 
	ThreadPool hreadPool(1);
	std::string url = "localhost:9999";
	std::string json = "asd";
	hreadPool.cast([&](){CCurlHttp()(url,json);});
	while(1);
	return 0;
}
