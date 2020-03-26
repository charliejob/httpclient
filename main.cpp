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
	for(int i=0;i<10;i++){
		for(int i=0;i<10;i++){
			hreadPool.cast([&](){std::cout << CCurlHttp()(url,json) << std::endl;});
		}
		std::cout << "before sleep" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}
