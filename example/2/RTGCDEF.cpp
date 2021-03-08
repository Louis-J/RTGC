#include<RTGC/RTGC.hpp>
#include<iostream>

std::atomic_flag RTGC::detail::GlobalMutex = ATOMIC_FLAG_INIT;

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



void RTGC::detail::GCThread::Start() {
    GlobalExitFlag = false;
    DebugStatus.store(100);
    DebugMutex.try_lock();
    DebugThread = std::thread([](){
        while(!GlobalExitFlag) {
            DebugMutex.try_lock_for(std::chrono::milliseconds(10000));
            auto val = DebugStatus.load();
            if(val < 0) {
                std::cout << "BAD!!! DebugStatus: " << val << "\n";
                // exit(val);
            } else
                DebugStatus.store(-val);
        }
    });
    std::cout << "START GC Thread\n";
}

void RTGC::detail::GCThread::End() {
    GlobalExitFlag = true;
    GlobalCV.notify_one();
    DebugMutex.unlock();
    DebugThread.join();
    std::cout << "END GC Thread\n";
}

void RTGC::detail::GCThread::Run() {
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