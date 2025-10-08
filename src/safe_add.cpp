#include <type_traits>
#include <iostream>
#include <string>

// 判断类型是否支持加法
template<typename T, typename = void>
struct HasAddition : std::false_type {};

template<typename T>
struct HasAddition<
    T,
    std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

// 仅当类型支持加法时才启用
template<typename T, typename = std::enable_if_t<HasAddition<T>::value, void>>
auto safe_add(T a, T b) {
    return a + b;
}

struct Foo {
    int x;
};

int main() {
    std::cout << safe_add(3, 5) << std::endl;                       // ✅ int
    std::cout << safe_add(std::string("hello "), std::string("world")) << std::endl; // ✅ string

    Foo a{1}, b{2};
    // safe_add(a, b); //  编译错误: Foo 没有 operator+

    return 0;
}
