#include <bits/stdc++.h>

class ThreadPool {
  public:

  private:
    std::vector<std::thread>workers;
    std::queue<std::function<void()>>tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::thread managerThread;

    size_t minThreads;
    size_t maxThreads;
    std::atomic<bool>stop;
    std::atomic<int32_t>liveCount;
    std::atomic<int32_t>busyCount;
    std::atomic<int32_t>exitCount{0};
};

int main() {
}
