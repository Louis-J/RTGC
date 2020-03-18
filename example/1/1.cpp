#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>

using namespace std;
using namespace RTGC;

class T1A;
class T1B;
class T2A;
class T2B;
class T3A;
class T3B;

class T1B {
    string str;
public:
    ShellPtr<T1A> next;
    T1B(string str):str(str){
        cout << str << " construct\n";
    }
    ~T1B() {
        cout << str << " destruct\n";
    }
public:
    CLASSLINK(T1B, 2)
};

class T1A {
    string str;
public:
    T1B next;
    T1A(string str1, string str2):str(str1), next(str2){
        cout << str << " construct\n";
    }
    ~T1A() {
        cout << str << " destruct\n";
    }
public:
    CLASSLINK(T1A, 2)
};



class T2B {
    string str;
public:
    shared_ptr<T2A> next;
    T2B(string str):str(str){
        cout << str << " construct\n";
    }
    ~T2B() {
        cout << str << " destruct\n";
    }
};

class T2A {
    string str;
public:
    T2B next;
    T2A(string str1, string str2):str(str1), next(str2){
        cout << str << " construct\n";
    }
    ~T2A() {
        cout << str << " destruct\n";
    }
};


class T3B {
    string str;
public:
    unique_ptr<T3A> next;
    T3B(string str):str(str){
        cout << str << " construct\n";
    }
    ~T3B() {
        cout << str << " destruct\n";
    }
};

class T3A {
    string str;
public:
    T3B next;
    T3A(string str1, string str2):str(str1), next(str2){
        cout << str << " construct\n";
    }
    ~T3A() {
        cout << str << " destruct\n";
    }
};

int main() {
    cout << "RTGC:" << endl;
    {
        ShellPtr<T1A> a1(MakeShell<T1A>("a1", "b1"));
        ShellPtr<T1A> a2(MakeShell<T1A>("a2", "b2"));
        a1->next.next = a2;
        a2->next.next = a1;
    }
    cout << "shared_ptr:" << endl;
    {
        shared_ptr<T2A> a1(make_shared<T2A>("a1", "b1"));
        shared_ptr<T2A> a2(make_shared<T2A>("a2", "b2"));
        a1->next.next = a2;
        a2->next.next = a1;
    }
    cout << "unique_ptr:" << endl;
    {
        unique_ptr<T2A> a1(make_unique<T2A>("a1", "b1"));
        unique_ptr<T2A> a2(make_unique<T2A>("a2", "b2"));
        a1->next.next = move(a2);
        a2->next.next = move(a1);
    }
}

