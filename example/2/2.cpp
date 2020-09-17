#include<iostream>
#include<thread>
#include<mutex>
#include<RTGC/RTGC.hpp>
#include<memory>
#include<vector>
// #include<variant>
#include<boost/variant2/variant.hpp>
using namespace boost::variant2;
#include<boost/timer/timer.hpp>

using namespace std;
using namespace RTGC;

// 稍复杂的示意, 展示了多线程时循环引用的情况

constexpr int ThreadNum = 50;
constexpr int ArrayNum = 1000;
constexpr int LoopNum = 100;

class T1 {
public:
    RTGC_AutoChainLink(T1, 1)
    static size_t cnsNum;
    static size_t desNum;
public:
    ChainPtr<T1> next;
    T1() {
        cnsNum++;
        cout << "construct\n";
    }
    
    ~T1() {
        desNum++;
        cout << "destruct\n";
    }
};
size_t T1::cnsNum = 0;
size_t T1::desNum = 0;

class T2 {
public:
    static size_t cnsNum;
    static size_t desNum;
public:
    shared_ptr<T2> next;
    T2() {
        cnsNum++;
        // cout << "construct\n";
    }
    
    ~T2() {
        desNum++;
        // cout << "destruct\n";
    }
};
size_t T2::cnsNum = 0;
size_t T2::desNum = 0;

variant<ChainPtr<T1>, shared_ptr<T2>> gPtr;
mutex gSync;
mutex gInit;
mutex gInitFinish;
int gnitNum;

template<typename Ptr>
class ThreadProc : public thread {
public:
    static void Init() {
        gInitFinish.lock();
    };
    ThreadProc() :thread([] {
        gInit.lock();
        vector<Ptr> arr(ArrayNum, get<Ptr>(gPtr));
        if(++gnitNum == ThreadNum) {
            gInitFinish.unlock();
        }
        gInit.unlock();

        gSync.lock();
        gSync.unlock();
        for (auto &i : arr) {
            i = nullptr;
        }
    }) {}
};

int main() {
    gInitFinish.lock();
    cout << "RTGC:" << endl;
    {
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LoopNum; i++) {
            gPtr = MakeChain<T1>();

            gnitNum = 0;
            gSync.lock();
            vector<ThreadProc<ChainPtr<T1>>> pool(ThreadNum);
            gInitFinish.lock();
            gPtr.emplace<ChainPtr<T1>>(nullptr);
            gSync.unlock();
            
            for(auto &j : pool) {
                j.join();
            }
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << endl; //输出：start()至调用此函数的系统时间
        cout << "cnsNum = " << T1::cnsNum << endl;
        cout << "desNum = " << T1::desNum << endl;
    }
    cout << endl;
    cout << "shared_ptr:" << endl;
    {
        boost::timer::cpu_timer t;
        t.start();
        for (int i = 0; i < LoopNum; i++) {
            gPtr = make_shared<T2>();

            gnitNum = 0;
            gSync.lock();
            vector<ThreadProc<shared_ptr<T2>>> pool(ThreadNum);
            gInitFinish.lock();
            gPtr.emplace<shared_ptr<T2>>(nullptr);
            gSync.unlock();
            
            for(auto &j : pool) {
                j.join();
            }
        }
        t.stop();
        cout << "all : " << t.elapsed().wall/1000000 << endl;   //输出：start()至调用此函数的经过时间
        cout << "user : " << t.elapsed().user/1000000 << endl;   //输出：start()至调用此函数的用户时间
        cout << "system : " << t.elapsed().system/1000000 << endl; //输出：start()至调用此函数的系统时间
        cout << "cnsNum = " << T2::cnsNum << endl;
        cout << "desNum = " << T2::desNum << endl;
    }
    

    return 0;
}