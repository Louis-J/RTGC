#include<RTGC/RTGC.hpp>
#include<iostream>
#include<chrono>

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
        // while(GlobalList.empty() || GlobalExitFlag)
            GlobalCV.wait(lk);

        // while(!GlobalList.empty()) {
        //     // std::cout << "RUN GC: 1\n";
        //     // DebugStatus.store(1);
        //     auto &node = GlobalList.front();
        //     // std::cout << "RUN GC: 2\n";
        //     // DebugStatus.store(2);
        //     node.Clear(DebugStatus);
        //     // std::cout << "RUN GC: 3\n";
        //     // DebugStatus.store(3);
        //     while(GlobalAtomic.test_and_set()); //得到GlobalAtomic
        //     // std::cout << "RUN GC: 4\n";
        //     // DebugStatus.store(4);
        //     GlobalList.pop_front();
        //     GlobalAtomic.clear();
        //     // std::cout << "RUN GC: 5\n";
        //     // DebugStatus.store(5);
        //     CountRun++;
        // }

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
    std::cout << "GC: CountRun: " << CountRun << "\n";
    std::cout << "GC: CountWake: " << CountWake << "\n";
}

void RTGC::detail::GCThread::Start() {
    GlobalExitFlag = false;
    DebugStatus.store(100);
    GlobalGCThread = std::thread(RTGC::detail::GCThread::Run);
    DebugMutex.try_lock();
    DebugThread = std::thread([](){
        while(!GlobalExitFlag) {
            DebugMutex.try_lock_for(std::chrono::milliseconds(5000));
            auto val = DebugStatus.load();
            if(val < 0) {
                std::cout << "BAD!!! DebugStatus: " << val << "\n";
            } else {
                DebugStatus.store(-val);
            }
        }
    });
    std::cout << "START GC Thread\n";
}
void RTGC::detail::GCThread::End() {
    GlobalExitFlag = true;
    GlobalCV.notify_one();
    DebugMutex.unlock();
    GlobalGCThread.join();
    DebugThread.join();
    std::cout << "END GC Thread\n";
}
