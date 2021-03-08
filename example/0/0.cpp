#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>

using namespace std;
using namespace RTGC;

// 简单示意, 比较了RTGC/shared_ptr/unique_ptr的安全性

class T1 {//use RTGC
private:
    string str;
public:
    constexpr static bool RTGC_MayCirRef = true;
    SmarterPtr<T1> next;
    T1(string &&str):str(str){
        cout << str << " construct\n";
    }
    ~T1() {
        cout << str << " destruct\n";
    }
public:
    RTGC_AutoChainLink(T1, 2);
    RTGC_AutoCRDetectIn(T1, 2);
};
RTGC_AutoCRDetectOut(T1);

class T2 {//use shared_ptr
private:
    string str;
public:
    shared_ptr<T2> next;
    T2(string &&str):str(str){
        cout << str << " construct\n";
    }
    ~T2() {
        cout << str << " destruct\n";
    }
};

class T3 {//use unique_ptr
private:
    string str;
public:
    unique_ptr<T3> next;
    T3(string &&str):str(str){
        cout << str << " construct\n";
    }
    ~T3() {
        cout << str << " destruct\n";
    }
};

int main() {
    cout << "Test1:" << endl;
    cout << "RTGC:" << endl;
    {
        ChainPtr<T1> tA(MakeChain<T1>("tA"));
        ChainPtr<T1> tB(MakeChain<T1>("tB"));
        tA->next = tB;
        tB->next = tA;
    }
    cout << "shared_ptr:" << endl;
    {
        shared_ptr<T2> tA(make_shared<T2>("tA"));
        shared_ptr<T2> tB(make_shared<T2>("tB"));
        tA->next = tB;
        tB->next = tA;
    }
    cout << "unique_ptr:" << endl;
    {
        unique_ptr<T3> tA(make_unique<T3>("tA"));
        unique_ptr<T3> tB(make_unique<T3>("tB"));
        tA->next = move(tB);
        tA->next->next = move(tA);
    }
    cout << endl;

    cout << "Test2:" << endl;
    cout << "RTGC:" << endl;
    {
        ChainPtr<T1> tA(MakeChain<T1>("tA"));
        tA->next = MakeChain<T1>("tB");
        tA->next->next = tA->next;
    }
    cout << "shared_ptr:" << endl;
    {
        shared_ptr<T2> tA(make_shared<T2>("tA"));
        tA->next = make_shared<T2>("tB");
        tA->next->next = tA->next;
    }
    cout << "unique_ptr:" << endl;
    {
        unique_ptr<T3> tA(make_unique<T3>("tA"));
        tA->next = make_unique<T3>("tB");
        tA->next->next = move(tA->next);
    }
    cout << endl;
    return 0;
}