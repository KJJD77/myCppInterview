#include <bits/stdc++.h>

std::atomic<int> count(0);

void increment(int n) {
    while (n--) count.fetch_add(1);
}

void decrement(int n) {
    while (n--) count.fetch_sub(1);
}

void cas_demo(int old_val, int new_val) {
    int expected = old_val;
    if (count.compare_exchange_strong(expected, new_val)) {
        std::cout << "CAS success: counter changed to " << new_val << std::endl;
    } else {
        std::cout << "CAS fail: counter is " << expected << std::endl;
    }
}

void exchange_demo(int new_val) {
    int old = count.exchange(new_val);
    std::cout << "Exchange: counter changed from " << old << " to " << new_val
              << std::endl;
}

int main() {
    std::vector<std::thread> tasks;
    // 启动多个自增线程
    for (int i = 0; i < 5; ++i) tasks.emplace_back(increment, 1000);

    // 启动多个自减线程
    for (int i = 0; i < 5; ++i) tasks.emplace_back(decrement, 500);

    // 启动 CAS 和 exchange 线程
    tasks.emplace_back(cas_demo, 0, 999);
    tasks.emplace_back(exchange_demo, 888);

    // 等待所有线程完成
    for (auto &t : tasks) t.join();

    std::cout << "Final counter: " << count.load() << std::endl;

    return 0;
}