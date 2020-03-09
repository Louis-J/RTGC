#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>

class T1 {//use RTGC
    CLASSLINK(T1, 2)
private:
    std::string str;
public:
    RTGC::ShellPtr<T1> next;
    T1(std::string &&str):str(str){}
    ~T1() {
        std::cout << str << " destructor\n";
    }
};

class T2 {//use shared_ptr
private:
    std::string str;
public:
    std::shared_ptr<T2> next;
    T2(std::string &&str):str(str){}
    ~T2() {
        std::cout << str << " destructor\n";
    }
};

int main() {
    std::cout << "RTGC:" << std::endl;
    {
        RTGC::ShellPtr<T1> t1(new RTGC::CorePtr<T1>("t1"));
        RTGC::ShellPtr<T1> t2(new RTGC::CorePtr<T1>("t2"));
        t1->next = t2;
        t2->next = t1;
    }
    std::cout << "shared_ptr:" << std::endl;
    {
        std::shared_ptr<T2> t1(new T2("t1"));
        std::shared_ptr<T2> t2(new T2("t2"));
        t1->next = t2;
        t2->next = t1;
    }
    return 0;
}