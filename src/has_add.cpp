#include <bits/stdc++.h> 

template<typename T>
auto has_true(T a,T b) -> decltype(std::declval<T>()+std::declval<T>(),std::true_type{}) {
    return std::true_type{};
}

template<typename T>
std::false_type has_true(...) {
    return std::false_type{};
}

template<typename T>
using HasAddition=decltype(has_true<T>(std::declval<T>(), std::declval<T>()));

struct Foo {}; 


int main() {
    std::cout << std::boolalpha;  
    std::cout << "int: " << HasAddition<int>::value << "\n";           
    std::cout << "double: " << HasAddition<double>::value << "\n";     
    std::cout << "std::string: " << HasAddition<std::string>::value << "\n";
    std::cout << "Foo: " << HasAddition<Foo>::value << "\n";  
}
