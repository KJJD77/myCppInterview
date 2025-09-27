#include <bits/stdc++.h>

class signleton {
  public:
    static signleton &get() { return signle; }

  private:
    signleton() {};
    signleton(const signleton &) = delete;
    signleton &operator=(const signleton &) = delete;
    static signleton signle;
};
signleton signleton::signle;
int main() {
    signleton &signle = signleton::get();
}