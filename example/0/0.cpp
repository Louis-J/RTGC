#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>

using namespace std;
using namespace RTGC;

// 简单示意, 比较了RTGC/shared_ptr/unique_ptr的安全性

class T1 {//use RTGC
public:
    constexpr static bool RTGC_MayCirRef = true;
    RTGC_AutoChainLink(T1, 2);
    RTGC_AutoCRDetectIn(T1, 2);
    
private:
    string str;
public:
    SmarterPtr<T1> next;
    T1(string &&str):str(str){
        cout << str << " construct\n";
    }
    ~T1() {
        cout << str << " destruct\n";
    }
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
    RTGC::detail::GCThread::Start();
    {
        SmarterPtr<T1> tA(MakeSmarter<T1>("tA"));
        SmarterPtr<T1> tB(MakeSmarter<T1>("tB"));
        tA->next = tB;
        tB->next = tA;
    }
    RTGC::detail::GCThread::End();
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
    RTGC::detail::GCThread::Start();
    {
        SmarterPtr<T1> tA(MakeSmarter<T1>("tA"));
        tA->next = MakeSmarter<T1>("tB");
        tA->next->next = tA->next;
    }
    RTGC::detail::GCThread::End();
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