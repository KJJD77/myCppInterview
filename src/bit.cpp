#include <bits/stdc++.h>

struct BIT {
    std::vector<int> t;
    int n;

    BIT(int n) : n(n), t(n + 1) {};

    void add(int idx, int v) {
        for (int i = idx; i <= n; i += (i & (-i))) t[i] += v;
    }

    int query(int idx) {
        int s = 0;
        for (int i = idx; i; i -= (i & (-i))) s += t[i];
        return s;
    }

    int range_query(int l, int r) { return query(r) - query(l - 1); }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    BIT bit(6);

    std::vector<int> a = {0, 2, 1, 3, 5, 4, 2};

    std::cout << "=== 构建 BIT ===\n";
    for (int i = 1; i <= 6; ++i) {
        bit.add(i, a[i]);
        std::cout << "add(" << i << ", " << a[i] << ")\n";
    }

    std::cout << "\n=== 前缀和测试 ===\n";
    for (int i = 1; i <= 6; ++i)
        std::cout << "sum(1.." << i << ") = " << bit.query(i) << "\n";

    std::cout << "\n=== 区间和测试 ===\n";
    std::vector<std::pair<int,int>> queries = {{1,3}, {2,5}, {4,6}, {3,3}};
    for (auto [l, r] : queries)
        std::cout << "sum(" << l << ".." << r << ") = " << bit.range_query(l, r) << "\n";

    std::cout << "\n=== 修改测试 ===\n";
    bit.add(3, +5); // a3 += 5
    std::cout << "add(3, +5)\n";
    std::cout << "sum(1..6) = " << bit.query(6) << "\n";
    std::cout << "sum(3..3) = " << bit.range_query(3, 3) << "\n";
    std::cout << "sum(2..4) = " << bit.range_query(2, 4) << "\n";

    return 0;
}