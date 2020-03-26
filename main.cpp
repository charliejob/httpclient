#include <stdio.h>
#include "httpclient.h"
#include "threadpool.h"
#include <iostream>
int main(){
	std::string res = CCurlHttp()("localhost:9999","asd");
	std::cout << "res: " << res << std::endl; 
	ThreadPool threadPool(1);
	std::string url = "localhost:9999";
	std::string json = "asd";
	threadPool.cast([&](){CCurlHttp()(url,json);});
	for(int i=0;i<10;i++){
		for(int i=0;i<10;i++){
			threadPool.cast([&](){std::cout << "cast result: " << CCurlHttp()(url,json) << std::endl;});
		}
		for(int i=0;i<10;i++){
			std::future<std::string> b = threadPool.commit([&]()->std::string {return CCurlHttp()(url,json); });
			std::cout << "commit result: " << b.get() << std::endl;
		}
		std::cout << "before sleep" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}
