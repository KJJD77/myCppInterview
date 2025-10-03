#include <bits/stdc++.h>
#include <future>

class ThreadPool {
  public:
    ThreadPool(size_t minThreads, size_t maxThreads)
        : minThreads(minThreads)
        , maxThreads(maxThreads)
        , stop(false)
        , liveCount(0)
        , busyCount(0) {
        for (size_t i = 0; i < minThreads; i++) createWorkers();
        managerThread = std::thread(&ThreadPool::manager, this);
    }
    template<typename F, typename... Args>
    auto enqueue(F &&f, Args... args)
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
    };

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
    void createWorkers() {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    this->condition.wait(lock, [this]() {
                        return this->stop || !this->tasks.empty()
                               || this->exitCount != 0;
                    });

                    if ((stop && tasks.empty())
                        || (exitCount > 0 && liveCount > (int)minThreads)) {
                        liveCount--;
                        if (exitCount) exitCount--;
                        return;
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
                qSize = tasks.size();
                busy = busyCount.load();
                live = liveCount.load();
            }

            if (qSize > live && live < (int)maxThreads) {
                int addCount = 0;
                for (size_t i = 0; addCount < 2 && liveCount < maxThreads;
                     i++) {
                    createWorkers();
                    addCount++;
                }
            }

            if (busy * 2 < live && live > (int)minThreads) {
                exitCount = 2; // 一次杀 2 个
                for (int i = 0; i < 2; i++) { condition.notify_one(); }
            }
        }
    }

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread managerThread;

    size_t minThreads;
    size_t maxThreads;
    std::atomic<bool> stop;
    std::atomic<int32_t> liveCount;
    std::atomic<int32_t> busyCount;
    std::atomic<int32_t> exitCount{0};
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
