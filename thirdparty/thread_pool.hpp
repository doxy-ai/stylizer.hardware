// FROM: https://github.com/ZenSepiol/Dear-ImGui-App-Framework
// MIT License

// Copyright (c) 2022 Zen Sepiol

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
// modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace ZenSepiol { // Modification
class ThreadPool
{
  public:
    ThreadPool(const int size) : busy_threads(size), threads(std::vector<std::thread>(size)), shutdown_requested(false)
    {
        for (size_t i = 0; i < size; ++i)
        {
            threads[i] = std::thread(ThreadWorker(this));
        }
    }

    ~ThreadPool()
    {
        Shutdown();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)      = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    // Waits until threads finish their current task and shutdowns the pool
    void Shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            shutdown_requested = true;
            condition_variable.notify_all();
        }

        for (size_t i = 0; i < threads.size(); ++i)
        {
            if (threads[i].joinable())
            {
                threads[i].join();
            }
        }
    }

    template <typename F, typename... Args>
    auto AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
#if __cpp_lib_move_only_function
        std::packaged_task<decltype(f(args...))()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto future       = task.get_future();
        auto wrapper_func = [task = move(task)]() mutable { std::move(task)(); };
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(wrapper_func));
            // Wake up one thread if its waiting
            condition_variable.notify_one();
        }

        // Return future from promise
        return future;
#else

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        auto wrapper_func = [task_ptr]() { (*task_ptr)(); };
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(wrapper_func);
            // Wake up one thread if its waiting
            condition_variable.notify_one();
        }

        // Return future from promise
        return task_ptr->get_future();
#endif
    }

    int QueueSize()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.size();
    }

  private:
    class ThreadWorker
    {
      public:
        ThreadWorker(ThreadPool* pool) : thread_pool(pool)
        {
        }

        void operator()()
        {
            std::unique_lock<std::mutex> lock(thread_pool->mutex);
            while (!thread_pool->shutdown_requested || (thread_pool->shutdown_requested && !thread_pool->queue.empty()))
            {
                thread_pool->busy_threads--;
                thread_pool->condition_variable.wait(lock, [this] {
                    return this->thread_pool->shutdown_requested || !this->thread_pool->queue.empty();
                });
                thread_pool->busy_threads++;

                if (!this->thread_pool->queue.empty())
                {
#if __cpp_lib_move_only_function
                    auto func = std::move(thread_pool->queue.front());
#else
                    auto func = thread_pool->queue.front();
#endif
                    thread_pool->queue.pop();

                    lock.unlock();
                    func();
                    lock.lock();
                }
            }
        }

      private:
        ThreadPool* thread_pool;
    };

  public:
    int busy_threads;

  private:
    mutable std::mutex mutex;
    std::condition_variable condition_variable;

    std::vector<std::thread> threads;
    bool shutdown_requested;

#if __cpp_lib_move_only_function
    std::queue<std::move_only_function<void()>> queue;
#else
    std::queue<std::function<void()>> queue;
#endif
};
} // Modification