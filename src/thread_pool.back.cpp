#include <bits/stdc++.h>

class ThreadPool {
  public:
    ThreadPool(size_t minThreads, size_t maxThreads)
        : minThreads(minThreads)
        , maxThreads(maxThreads)
        , stop(false)
        , busyCount(0)
        , liveCount(0) {
        for (size_t i = 0; i < minThreads; ++i) { createWorker(); }
        managerThread = std::thread(&ThreadPool::manager, this);
    }

    template<class F, class... Args>
    auto enqueue(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();

        if (managerThread.joinable()) managerThread.join();

        for (std::thread &worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread managerThread;

    size_t minThreads;
    size_t maxThreads;
    std::atomic<bool> stop;
    std::atomic<int> busyCount;
    std::atomic<int> liveCount;
    std::atomic<int> exitCount{0}; // 待销毁线程数

  private:
    void createWorker() {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty()
                               || this->exitCount > 0;
                    });

                    if ((stop && tasks.empty())
                        || (exitCount > 0 && liveCount > (int)minThreads)) {
                        --liveCount;
                        if (exitCount > 0) --exitCount;
                        return; // 线程退出
                    }

                    if (!tasks.empty()) {
                        task = std::move(tasks.front());
                        tasks.pop();
                        ++busyCount;
                    }
                }

                if (task) {
                    task();
                    --busyCount;
                }
            }
        });
        ++liveCount;
    }

    void manager() {
        while (!stop) {
            std::this_thread::sleep_for(std::chrono::seconds(3));

            int qSize, busy, live;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                qSize = (int)tasks.size();
                busy = busyCount.load();
                live = liveCount.load();
            }

            // 扩容逻辑：任务多，线程不够
            if (qSize > live && live < (int)maxThreads) {
                int addCount = 0;
                for (size_t i = 0; i < maxThreads && addCount < 2
                                   && liveCount < (int)maxThreads;
                     ++i) {
                    createWorker();
                    addCount++;
                }
            }

            // 缩减逻辑：空闲线程过多
            if (busy * 2 < live && live > (int)minThreads) {
                exitCount = 2; // 一次杀 2 个
                for (int i = 0; i < 2; i++) { condition.notify_one(); }
            }
        }
    }
};

int main() {
    ThreadPool pool(2, 6); // 最少2线程，最多6线程

    std::vector<std::future<int>> results;
    for (int i = 0; i < 12; ++i) {
        results.emplace_back(pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "task " << i << " done by thread "
                      << std::this_thread::get_id() << std::endl;
            return i * i;
        }));
    }

    for (auto &res : results) {
        std::cout << "result = " << res.get() << std::endl;
    }

    return 0;
}
