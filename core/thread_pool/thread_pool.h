#ifndef HTTP_LIB_THREAD_POOL_H
#define HTTP_LIB_THREAD_POOL_H

#include <iostream>
#include <atomic>

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stop(false) {
        for (size_t i = 0; i < numThreads; ++i) {//在workers中加入numThreads个线程
            workers.emplace_back([this] {//线程的执行函数是从tasks中获取一个task执行
                while (true) {
                    std::function<void()> task;//创建一个task

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);//对任务队列加锁
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        //如果stop为true或tasks不为空，继续执行，否则释放锁线程被挂起，在其他进程调用condition.notify_one() 或 condition.notify_all()时进程被唤醒，重新获取锁执行
                        //如果继续执行则等到线程结束后释放锁
                        if (stop && tasks.empty()) {//没有任务但stop为true，终止循环
                            return;
                        }

                        task = std::move(tasks.front());//从任务队列中获取任务
                        tasks.pop();
                    }

                    task();//执行任务
                }
            });
        }
    }

    //将任务加入到tasks中
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));//创建任务，异步调用任务
        std::future<return_type> result = task->get_future();//获取异步任务处理结果

        //括号内代码被锁保护，离开代码块lock变量被析构，释放锁
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); });//将任务加入队列
        }

        condition.notify_one();//唤醒一个线程执行该任务
        cout << "end!" << endl;
        return result;//返回异步任务处理结果
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;//通知线程池的线程停止运行
        }

        condition.notify_all();//唤醒所有线程

        for (std::thread &worker : workers) {
            worker.join();//等待所有线程执行完毕
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic_bool stop;
};

#endif //HTTP_LIB_THREAD_POOL_H
