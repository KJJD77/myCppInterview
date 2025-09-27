#include <bits/stdc++.h>

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

class shoeFactory {
  public:
    virtual std::unique_ptr<iShoe> create() const = 0;
    virtual ~shoeFactory() = default;
};

class nikeShoeFactory : public shoeFactory {
  public:
    virtual std::unique_ptr<iShoe> create() const override {
        return std::make_unique<nikeShoe>();
    }
};

class adidasShoeFactory : public shoeFactory {
  public:
    virtual std::unique_ptr<iShoe> create() const override {
        return std::make_unique<adidasShoe>();
    }
};

int main() {
    std::unique_ptr<shoeFactory> f1 = std::make_unique<nikeShoeFactory>();
    std::unique_ptr<shoeFactory> f2 = std::make_unique<adidasShoeFactory>();

    auto s1 = f1->create();
    auto s2 = f2->create();
    s1->wear();
    s2->wear();
}