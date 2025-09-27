#include <bits/stdc++.h>

enum class brand { nike, adidas };

class iShoe {
  public:
    virtual void wear() const = 0;
    virtual ~iShoe() = default;
};

class nikeShoe : public iShoe {
  public:
    virtual void wear() const override { std::cout << "i wear nike\n"; }
};

class adidasShoe : public iShoe {
  public:
    virtual void wear() const override { std::cout << "i wear adidas\n"; }
};

class simpleShoeFactory {
  public:
    static std::unique_ptr<iShoe> create(brand b) {
        if (b == brand::adidas)
            return std::make_unique<adidasShoe>();
        else if (b == brand::nike)
            return std::make_unique<nikeShoe>();
        else
            return nullptr;
    }
};

int main() {
    auto s1 = simpleShoeFactory::create(brand::adidas);
    s1->wear();
    auto s2 = simpleShoeFactory::create(brand::nike);
    s2->wear();
}