#include<RTGC/RTGC.hpp>
#include<iostream>

std::list<RTGC::detail::GCThread::GCNode>   RTGC::detail::GCThread::GlobalList;
std::atomic_flag                            RTGC::detail::GCThread::GlobalAtomic = ATOMIC_FLAG_INIT;
std::mutex                                  RTGC::detail::GCThread::GlobalMutex;
std::condition_variable                     RTGC::detail::GCThread::GlobalCV;
bool                                        RTGC::detail::GCThread::GlobalExitFlag = false;
std::thread                                 RTGC::detail::GCThread::GlobalGCThread;


void RTGC::detail::GCThread::Run() {
    while(!GlobalExitFlag) {
        std::unique_lock<std::mutex> lk(GlobalMutex);
        // while(GlobalList.empty() || GlobalExitFlag)
            GlobalCV.wait(lk);

        while(!GlobalList.empty()) {
            // std::cout << "RUN GC\n";
            auto &node = GlobalList.front();
            node.Clear();
            while(GlobalAtomic.test_and_set()); //得到GlobalAtomic
            GlobalList.pop_front();
            GlobalAtomic.clear();
        }
    }
    for(auto &i: GlobalList)
        i.Clear();
    GlobalList.clear();
    // std::cout << "END GC Thread, " << GlobalList.size() << " node not clear\n";
}

void RTGC::detail::GCThread::Start() {
    GlobalExitFlag = false;
    GlobalGCThread = std::thread(RTGC::detail::GCThread::Run);
    std::cout << "START GC Thread\n";
}
void RTGC::detail::GCThread::End() {
    GlobalExitFlag = true;
    GlobalCV.notify_one();
    GlobalGCThread.join();
    std::cout << "END GC Thread\n";
}
