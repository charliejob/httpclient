#pragma once
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#define THREADPOOL_MAX_NUM 10
class ThreadPool
{
	using Task = std::function<void()>;
public:
	ThreadPool(unsigned short size=THREADPOOL_MAX_NUM) {
		_addThread(size);
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

