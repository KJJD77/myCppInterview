#include <bits/stdc++.h>
std::queue<int> buffer;
std::mutex mutex;
std::condition_variable cv;
const size_t MAX_BUFFER = 1;
void producer(int id, int count) {
    for (int i = 0; i < count; i++) {
        std::unique_lock lock(mutex);
        while (buffer.size() >= MAX_BUFFER) cv.wait(lock);
        buffer.push(i);
        std::cout << "producer id : " << id << "  produced " << i << std::endl;
        cv.notify_one();
    }
}

void consumer(int id, int count) {
    for (int i = 0; i < count; i++) {
        std::unique_lock lock(mutex);
        while (buffer.size() == 0) cv.wait(lock);
        auto now = buffer.front();
        buffer.pop();
        std::cout << "consumer id : " << id << "  consume " << now << std::endl;
        cv.notify_one();
    }
}

int main() {
    std::thread t1(producer, 1, 1000);
    std::thread t2(consumer, 1, 1000);
    t1.join();
    t2.join();
}