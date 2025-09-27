#include <bits/stdc++.h>

class iShoe {
  public:
    virtual void wear() const = 0;
    virtual ~iShoe() = default;
};

class iJacket {
  public:
    virtual void putOn() const = 0;
    virtual ~iJacket() = default;
};

class nikeShoe : public iShoe {
  public:
    virtual void wear() const override { std::cout << "i wear nike\n"; }
};

class nikeJacket : public iJacket {
  public:
    virtual void putOn() const override { std::cout << "i putOn nike\n"; }
};

class adidasShoe : public iShoe {
  public:
    virtual void wear() const override { std::cout << "i wear adidas\n"; }
};

class adidasJacket : public iJacket {
  public:
    virtual void putOn() const override { std::cout << "i putOn adidas\n"; }
};
class factory {
  public:
    virtual std::unique_ptr<iShoe> createShoe() const = 0;
    virtual std::unique_ptr<iJacket> createJacket() const = 0;
    virtual ~factory() = default;
};

class nikeShoeFactory : public factory {
  public:
    virtual std::unique_ptr<iShoe> createShoe() const override {
        return std::make_unique<nikeShoe>();
    }
    virtual std::unique_ptr<iJacket> createJacket() const override {
        return std::make_unique<nikeJacket>();
    }
};

class adidasShoeFactory : public factory {
  public:
    virtual std::unique_ptr<iShoe> createShoe() const override {
        return std::make_unique<adidasShoe>();
    }
    virtual std::unique_ptr<iJacket> createJacket() const override {
        return std::make_unique<adidasJacket>();
    }
};

int main() {
    std::unique_ptr<factory> nikeFactory = std::make_unique<nikeShoeFactory>();
    auto nikeShoe = nikeFactory->createShoe();
    auto nikeJacket = nikeFactory->createJacket();

    nikeShoe->wear();
    nikeJacket->putOn();

    std::cout << "-----------------\n";

    std::unique_ptr<factory> adidasFactory =
        std::make_unique<adidasShoeFactory>();
    auto adidasShoe = adidasFactory->createShoe();
    auto adidasJacket = adidasFactory->createJacket();

    adidasShoe->wear();
    adidasJacket->putOn();
    return 0;
}
