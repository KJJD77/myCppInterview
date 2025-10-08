#include <bits/stdc++.h>

template<typename T, size_t N, size_t... I>
struct reverse_array_helper {
    using type = typename reverse_array_helper<T, N - 1, I..., N - 1>::type;
    inline static constexpr type value =
        reverse_array_helper<T, N - 1, I..., N - 1>::value;
};

template<typename T, size_t... I>
struct reverse_array_helper<T, 0, I...> {
    using type = std::array<T, sizeof...(I)>;
    inline static constexpr type value = {static_cast<T>(I)...};
};

template<typename T, size_t N>
using ReverseArray = reverse_array_helper<T, N>;

int main() {
    constexpr auto arr = ReverseArray<int, 5>::value;
    for (auto v : arr) std::cout << v << ' ';
    std::cout << '\n';
}
