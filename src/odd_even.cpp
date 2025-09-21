#include <bits/stdc++.h>

std::condition_variable cv;
std::mutex mutex;
bool odd_turn = true;

void PrintOdd(int num) {
    for (int i = 1; i <= num; i += 2) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, []() { return odd_turn; });
        std::cout << "PrintOdd : " << i << '\n';
        odd_turn = false;
        cv.notify_all();
    }
}

void PrintEven(int num) {
    for (int i = 2; i <= num; i += 2) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, []() { return !odd_turn; });
        std::cout << "PrintEven : " << i << '\n';
        odd_turn = true;
        cv.notify_all();
    }
}

int main() {
    std::thread t1(PrintOdd,100);
    std::thread t2(PrintEven,100);
    t1.join();
    t2.join();
}