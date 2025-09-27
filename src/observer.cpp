#include <bits/stdc++.h>

class observer {
  public:
    virtual void update() = 0;
    virtual ~observer() {

    };
};

class subject {
  public:
    void attach(observer *obs) { observers.push_back(obs); }

    void detach(observer *obs) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), obs),
            observers.end());
    }

  protected:
    void notify() {
        for (auto *obs : observers) {
            if (obs) obs->update();
        }
    }

  private:
    std::vector<observer *> observers;
};
// 具体被观察者
class concreteSubject : public subject {
  public:
    void doSomething() {
        std::cout << "被观察者状态发生变化\n";
        notify(); // 通知所有观察者
    }
};

// 具体观察者1
class concreteObserver1 : public observer {
  public:
    void update() override { std::cout << "观察者1 收到通知并处理\n"; }
};

// 具体观察者2
class concreteObserver2 : public observer {
  public:
    void update() override { std::cout << "观察者2 收到通知并处理\n"; }
};

int main() {
    concreteSubject subject;

    concreteObserver1 obs1;
    concreteObserver2 obs2;

    subject.attach(&obs1);
    subject.attach(&obs2);

    subject.doSomething();

    return 0;
}