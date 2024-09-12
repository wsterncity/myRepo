﻿#ifndef iGameThreadPool_h
#define iGameThreadPool_h

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "iGameObject.h"

IGAME_NAMESPACE_BEGIN
class ThreadPool : public Object {
public:
    I_OBJECT(ThreadPool);

    // 单例
    static Pointer Instance() {
        static Pointer ins = new ThreadPool;
        return ins;
    }

    using Task = std::packaged_task<void()>;

    template <class F, class... Args>
    auto Commit(F&& f, Args&&... args) ->
        std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
        using RetType = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        if (stop_.load())
            return std::future<RetType>{};

        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<RetType> ret = task->get_future();
        {
            std::lock_guard<std::mutex> cv_mt(cv_mt_);
            tasks_.emplace([task] { (*task)(); });
        }
        cv_lock_.notify_one();
        return ret;
    }

    int IdleThreadCount() {
        return thread_num_;
    }

private:
    ThreadPool(unsigned int num = std::thread::hardware_concurrency())
        : stop_(false) 
    {
        if (num <= 1)
            thread_num_ = 2;
        else
            thread_num_ = num;
        this->Start();
    }

    ~ThreadPool() {
        this->Stop();
    }

    void Start() {
        for (int i = 0; i < thread_num_; ++i) {
            pool_.emplace_back([this]() {
                // 如果停止则退出循环
                while (!this->stop_.load()) { // while(stop_ == false)
                    Task task;
                    {
                        std::unique_lock<std::mutex> cv_mt(cv_mt_);
                        // 如果stop == false && tasks_.empty()，条件变量wait不会返回，线程将挂起。
                        this->cv_lock_.wait(cv_mt, [this] {
                            return this->stop_.load() || !this->tasks_.empty();
                            });
                        if (this->tasks_.empty())
                            return;

                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    this->thread_num_--;
                    task();
                    this->thread_num_++;
                }
                });
        }
    }

    void Stop() {
        stop_.store(true);
        cv_lock_.notify_all();
        for (auto& td : pool_) {
            if (td.joinable()) {
                //std::cout << "join thread " << td.get_id() << std::endl;
                td.join();
            }
        }
    }

private:
    std::mutex               cv_mt_;
    std::condition_variable  cv_lock_;
    std::atomic_bool         stop_;          // 线程池是否退出
    std::atomic_int          thread_num_;    // 空闲的线程数
    std::queue<Task>         tasks_;         // 任务队列
    std::vector<std::thread> pool_;          // 线程队列

};

IGAME_NAMESPACE_END
#endif