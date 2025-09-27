#include<bits/stdc++.h>

class signleton
{
public:
    static signleton& get()
    {
        static signleton signle;
        return signle;
    }
private:
    signleton(){};
    signleton(const signleton&) = delete;
    signleton& operator=(const signleton&) = delete;
};

int main()
{
    signleton& signle=signleton::get();
}