#include <bits/stdc++.h>

template<typename productType_t>
class iProductRegister {
  public:
    virtual productType_t *createProduct() = 0;

  protected:
    iProductRegister() {};
    virtual ~iProductRegister() {};

  private:
    iProductRegister(const iProductRegister &) = delete;
    iProductRegister &operator=(const iProductRegister &) = delete;
};

template<typename productType_t>
class productFactory {
  public:
    static productFactory<productType_t> &instance() {
        static productFactory<productType_t> productInstance;
        return productInstance;
    }

    void registerProduct(
        iProductRegister<productType_t> *productRegister, std::string name) {
        mProductRegistry[name] = productRegister;
    }

    productType_t *getProduct(std::string name) {
        if (mProductRegistry.find(name) != mProductRegistry.end()) {
            return mProductRegistry[name]->createProduct();
        }
        std::cout << "No product found for " << name << std::endl;
        return nullptr;
    }

  private:
    productFactory() {}
    ~productFactory() {}

    productFactory(const productFactory &) = delete;
    productFactory &operator=(const productFactory &) = delete;
    std::map<std::string, iProductRegister<productType_t> *> mProductRegistry;
};

template<typename productType_t, typename productImpl_t>
class productRegistrar : public iProductRegister<productType_t> {
  public:
    explicit productRegistrar(std::string name) {
        productFactory<productType_t>::instance().registerProduct(this, name);
    }
    virtual productType_t *createProduct() override {
        return new productImpl_t();
    }
};

class product {
  public:
    virtual void show() = 0;
    virtual ~product() {}
};

class productA : public product {
  public:
    void show() override { std::cout << "I'm ProductA" << std::endl; }
};

class productB : public product {
  public:
    void show() override { std::cout << "I'm ProductB" << std::endl; }
};

productRegistrar<product, productA> g_regA("A");
productRegistrar<product, productB> g_regB("B");

int main() {
    product* p1 = productFactory<product>::instance().getProduct("A");
    if (p1) {
        p1->show();  // I'm ProductA
        delete p1;
    }

    product* p2 = productFactory<product>::instance().getProduct("B");
    if (p2) {
        p2->show();  // I'm ProductB
        delete p2;
    }

    product* p3 = productFactory<product>::instance().getProduct("C");
    if (p3) {
        p3->show();
        delete p3;
    }
}
