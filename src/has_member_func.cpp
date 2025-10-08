#include <type_traits>
#include <iostream>

template <typename T, typename = void>
struct HasMemberFunction : std::false_type {};

template <typename T>
struct HasMemberFunction<T, std::void_t<decltype(std::declval<T>().member_function())>>
    : std::true_type {};

struct ClassWithFunction {
    void member_function() {
        std::cout << "ClassWithFunction::member_function" << std::endl;
    }
};

struct ClassWithoutFunction {};

int main() {
    static_assert(HasMemberFunction<ClassWithFunction>::value == true, "ClassWithFunction should have member_function");
    static_assert(HasMemberFunction<ClassWithoutFunction>::value == false, "ClassWithoutFunction should not have member_function");
    return 0;
}