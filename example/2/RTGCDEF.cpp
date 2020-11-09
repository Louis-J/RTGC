#include<RTGC/RTGC.hpp>
#include<iostream>

std::list<RTGC::detail::GCThread::GCNode>   RTGC::detail::GCThread::GlobalList;
std::atomic_flag                            RTGC::detail::GCThread::GlobalAtomic = ATOMIC_FLAG_INIT;
std::mutex                                  RTGC::detail::GCThread::GlobalMutex;
std::condition_variable                     RTGC::detail::GCThread::GlobalCV;
bool                                        RTGC::detail::GCThread::GlobalExitFlag = false;
std::thread                                 RTGC::detail::GCThread::GlobalGCThread;
size_t                                      RTGC::detail::GCThread::CountRun;
size_t                                      RTGC::detail::GCThread::CountWake;

std::atomic<int>                            RTGC::detail::GCThread::DebugStatus;
std::thread                                 RTGC::detail::GCThread::DebugThread;
std::timed_mutex                            RTGC::detail::GCThread::DebugMutex;



void RTGC::detail::GCThread::Run() {
    while(!GlobalExitFlag) {
        std::unique_lock<std::mutex> lk(GlobalMutex);
        GlobalCV.wait(lk);

// 1
        // while(!GlobalList.empty()) {
        //     // std::cout << "RUN GC\n";
        //     auto &node = GlobalList.front();
        //     node.Clear(DebugStatus);
        //     while(GlobalAtomic.test_and_set()); //得到GlobalAtomic
        //     GlobalList.pop_front();
        //     GlobalAtomic.clear();
        // }

// 2
        while(GlobalAtomic.test_and_set());
        while(true) {
            if(GlobalList.empty()) {
                GlobalAtomic.clear();
                break;
            }
            auto &node = GlobalList.front();
            GlobalAtomic.clear();
            node.Clear(DebugStatus);
            while(GlobalAtomic.test_and_set());
            GlobalList.pop_front();
        }
    }
    for(auto &i: GlobalList)
        i.Clear(DebugStatus);
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
