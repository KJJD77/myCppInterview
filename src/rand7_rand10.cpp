#include <bits/stdc++.h>
int rand7() {
    return rand() % 7 + 1;
}
int rand10() {
    while (true) {
        int a = rand7();
        int b = rand7();
        int sum = (a - 1) * 7 + b;
        if (sum <= 40) return 1 + (sum - 1) % 10;
    }
}
int main() {
    srand(time(0));
    std::cout << rand10()<<std::endl;
}
