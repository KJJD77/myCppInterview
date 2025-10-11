#include <type_traits>  // true_type, false_type, void_t
#include <iostream>
#include <vector>
#include <string>

template <typename, typename = std::void_t<> >
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

struct A {
    void size() {}   
};

struct B {
};

template<typename T>
void print_has_size() {
    std::cout << std::boolalpha
              << "has_size<" << typeid(T).name() << ">::value = "
              << has_size<T>::value << "\n";
}

int main() {
    print_has_size<std::string>();  
    print_has_size<std::vector<int>>(); 
    print_has_size<int>();      
    print_has_size<A>();        
    print_has_size<B>();      

    static_assert(has_size<A>::value == true,  "A should have size()");
    static_assert(has_size<B>::value == false, "B should not have size()");
}
