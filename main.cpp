#include <stdio.h>
#include "httpclient.h"
#include "threadpool.h"
#include <iostream>

std::string foo(std::string& url,std::string& data)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   return url+data;
}

template<typename T>
static void bind_pushData(T& pool,const string& pushData)
{
	if(g_ResultPushUrl.empty()){
		return;
	}
	if(pushData.empty()){
		return;
	}
	if(!pool)
	{
		return;
	}
	pool->enqueue([=](){ CCurlHttp()(g_ResultPushUrl,pushData); });
}

int main(){

	shared_ptr<ThreadPool> g_pool = make_shared<ThreadPool> (10);
	bind_pushData(g_pool,"asd");

	// std::string res = CCurlHttp()("localhost:9999","asd");
	// std::cout << "res: " << res << std::endl; 
	// ThreadPool threadPool(1);
	// std::string url = "localhost:9999";
	// std::string json = "asd";
	// threadPool.cast([&](){std::bind(&CCurlHttp()(url,json));});
	// for(int i=0;i<10;i++){
	// 	for(int i=0;i<10;i++){
	// 		threadPool.cast([&](){std::cout << "cast result: " << foo(url,json) << std::endl;});
	// 	}
	// 	// for(int i=0;i<10;i++){
	// 	// 	std::future<std::string> b = threadPool.commit([&]()->std::string {return foo(url,json); });
	// 	// 	std::cout << "commit result: " << b.get() << std::endl;
	// 	// }
	// 	std::cout << "before sleep" << std::endl;
		
	// }
	// std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return 0;
}
