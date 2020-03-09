#include<iostream>
#include<memory>
#include<vector>
#include<RTGC/RTGC.hpp>
#include<thread>
#include<mutex>

using namespace std;
class T1 {
    CLASSLINK(T1, 1)
    static size_t cnsNum;
    static size_t desNum;
public:
    RTGC::ShellPtr<T1> next;
    T1() {
        cnsNum++;
        // std::cout << "construct\n";
    }
    
    ~T1() {
        desNum++;
        // std::cout << "destruct\n";
    }
};
size_t T1::cnsNum = 0;
size_t T1::desNum = 0;


mutex gSync;
mutex gInit;
mutex gInitFinish;
int gnitNum;

RTGC::ShellPtr<T1> gPtr;
int ThreadProc() {
    gInit.lock();
    vector<RTGC::ShellPtr<T1>> arr(100, gPtr);
    if(++gnitNum == 4) {
        gInitFinish.unlock();
    }
    gInit.unlock();

    gSync.lock();
    gSync.unlock();
    for (auto &i : arr) {
        i = nullptr;
    }
    // gSync.unlock();
    return 0;
}

int main() {
    gInitFinish.lock();
    for (int i = 0; i < 1000; i++) {
        gPtr = new RTGC::CorePtr<T1>{};

        gnitNum = 0;
        gSync.lock();
        thread a(ThreadProc);
        thread b(ThreadProc);
        thread c(ThreadProc);
        thread d(ThreadProc);
        gInitFinish.lock();
        gPtr = nullptr;
        gSync.unlock();
        
        a.join();
        b.join();
        c.join();
        d.join();
    }
    
    cout << "cnsNum = " << T1::cnsNum << endl;
    cout << "desNum = " << T1::desNum << endl;

    return 0;
}