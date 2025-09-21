#include <bits/stdc++.h>

template<typename Derived>
class Shape {
  public:
    void drawTwice() const {
        static_assert(
            std::is_same<decltype(std::declval<Derived>().draw()), void>::value,
            "Derived must implement void draw() method");
        static_cast<const Derived *>(this)->draw();
        static_cast<const Derived *>(this)->draw();
    }
};

class Circle : public Shape<Circle> {
  public:
    void draw() const { std::cout << "circle draw\n"; }
};

class Rectangle : public Shape<Rectangle> {
  public:
    void draw() const { std::cout << "rectangle draw\n"; }
};

int main() {
    Circle c;
    Rectangle r;

    c.drawTwice(); // 调用父类提供的通用方法，实际执行子类逻辑
    r.drawTwice();
    return 0;
}
