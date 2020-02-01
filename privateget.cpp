#include<iostream>
#include<string>
#include<memory>
#include<RTGC.hpp>

using namespace std;

class Test {
private:
    int i;
    char c;

public:
    double d;
    float f;
    void print() {
        auto [x,y,z,w] = *this;
        cout << x << y << z << w;
    }
};

int main() {
    Test a;
    a.print();
    return 0;
}

