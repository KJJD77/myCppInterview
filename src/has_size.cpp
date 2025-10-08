#include <type_traits>  // true_type, false_type, void_t
#include <iostream>
#include <vector>
#include <string>

// ✅ 1. 默认模板（primary template）
// 假设所有类型都 *没有* size() 函数
template <typename, typename = std::void_t<> >
struct has_size : std::false_type {};

// ✅ 2. 偏特化版本
// 如果 decltype(std::declval<T>().size()) 合法，则匹配这个模板
template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

// ✅ 3. 定义两个测试类
struct A {
    void size() {}   // ✅ 有成员函数 size()
};

struct B {
    // ❌ 没有 size()
};

// ✅ 4. 测试函数模板
template<typename T>
void print_has_size() {
    std::cout << std::boolalpha
              << "has_size<" << typeid(T).name() << ">::value = "
              << has_size<T>::value << "\n";
}

int main() {
    // C++标准库类型
    print_has_size<std::string>();  // ✅ 有 size()
    print_has_size<std::vector<int>>(); // ✅ 有 size()
    print_has_size<int>();          // ❌ 没有 size()
    // 自定义类型
    print_has_size<A>();            // ✅ 有 size()
    print_has_size<B>();            // ❌ 没有 size()

    // 编译期验证（不需要运行也能检查）
    static_assert(has_size<A>::value == true,  "A should have size()");
    static_assert(has_size<B>::value == false, "B should not have size()");
}
