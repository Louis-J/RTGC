#include<iostream>
#include<memory>
#include<string>
#include<RTGC/RTGC.hpp>
#include<boost/timer.hpp>


class T7 {//use RTGC
public:
    CLASSLINK(T7, 2)

private:
    std::string str;
public:
    RTGC::ShellPtr<T7> next;
    T7(std::string &&str):str(str){}
    ~T7() {
        std::cout << str << " destructor\n";
    }
};

#define LOOPSIZE 10000000
int main() {
    // std::cout << "Test1" << std::endl;
    // {
    //     const boost::timer timeBegin;
    //     for (int i = 0; i < LOOPSIZE; i++) {
    //         std::shared_ptr<T7> a = std::make_shared<T7>("hahaha");
    //     }
    //     const uint32_t timeBy = timeBegin.elapsed() * 1000;
    //     std::cout << "time1:" << timeBy << "ms" << std::endl;
    // }
    // std::cout << "Test2" << std::endl;
    // {
    //     const boost::timer timeBegin;
    //     for (int i = 0; i < LOOPSIZE; i++) {
    //         auto a = std::make_shared<T7>("hahaha");
    //     }
    //     const uint32_t timeBy = timeBegin.elapsed() * 1000;
    //     std::cout << "time2:" << timeBy << "ms" << std::endl;
    // }
    // std::cout << "Test3" << std::endl;
    // {
    //     const boost::timer timeBegin;
    //     for (int i = 0; i < LOOPSIZE; i++) {
    //         auto a = RTGC::MakeShell<T7>("hahaha");
    //     }
    //     const uint32_t timeBy = timeBegin.elapsed() * 1000;
    //     std::cout << "time3:" << timeBy << "ms" << std::endl;
    // }
    // std::cout << "Test4" << std::endl;
    // {
    //     const boost::timer timeBegin;
    //     for (int i = 0; i < LOOPSIZE; i++) {
    //         RTGC::ShellPtr<T7> a = RTGC::MakeShell<T7>("hahaha");
    //     }
    //     const uint32_t timeBy = timeBegin.elapsed() * 1000;
    //     std::cout << "time4:" << timeBy << "ms" << std::endl;
    // }
    std::cout << "RTGC:" << std::endl;
    {
        auto tD(RTGC::MakeShell<T7>("tD"));
        RTGC::ShellPtr<T7> tA(RTGC::MakeShell<T7>("tA"));
        tA->next = RTGC::MakeShell<T7>("tB");
        tA->next->next = tA->next;
    }
    return 0;
}