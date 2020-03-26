#pragma once
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <algorithm>
#define THREADPOOL_MAX_NUM 10
class ThreadPool
{
	using Task = std::function<void()>;
public:
	ThreadPool(unsigned short size=THREADPOOL_MAX_NUM) {
		_addThread(std::min(size, (decltype(size))THREADPOOL_MAX_NUM));
	};
	~ThreadPool() {
		_run = false;
		_task_cv.notify_all();
		for (std::thread& th : _pool) {
			//th.detach();
			if (th.joinable())
				th.join();
		}
	};
	void cast(Task&& f)
	{
		{
			std::lock_guard<std::mutex> lock(_lock);
			_tasks.emplace([f]() {
				f();
			});
		}
		_task_cv.notify_one();
	};
	template<class F>
	auto commit(F&& f)->std::future<decltype(f())>
	{
		using RetType = decltype(f());
		auto task = std::make_shared<std::packaged_task<RetType()>>(f);
		std::future<RetType> fut = task->get_future();
		{
			std::lock_guard<std::mutex> lock(_lock);
			_tasks.emplace([task]() {
				(*task)();
			});
		}
		_task_cv.notify_one();
		return fut;
	}
	//bind： .commit(std::bind(&Dog::sayHello, &dog));
	//mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
	template<class F, class... Args>
	inline auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
	{
		return commit(bind(std::forward<F>(f), std::forward<Args>(args)...));
	}
private:
	void _addThread(unsigned short size)
	{
		for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
		{
			_pool.emplace_back([this] {
				while (_run)
				{
					Task task;
					{
						std::unique_lock<std::mutex> lock{ _lock };
						_task_cv.wait(lock, [this] {
							return !_run || !_tasks.empty();
						});
						if (!_run && _tasks.empty())
							return;
						task = std::move(_tasks.front());
						_tasks.pop();
					}
					task();
				}
			});
		};
	}
private:
	std::vector<std::thread> _pool;
	std::queue<Task> _tasks;
	std::mutex _lock;
	std::condition_variable _task_cv;
	std::atomic<bool> _run{ true };
};

