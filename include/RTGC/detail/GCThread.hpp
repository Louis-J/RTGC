#ifndef RTGC_DETAIL_GCTHREAD_HPP
#define RTGC_DETAIL_GCTHREAD_HPP

#include<cstddef>
#include<atomic>
#include<thread>
#include<mutex>
#include<list>
#include<vector>
#include<condition_variable>

namespace RTGC { namespace detail {

template<typename T>
class ChainPtr;

class GCThread {            
    // constexpr size_t firstListLength = 32;
    // constexpr size_t LengthAdd(size_t lenNow) {
    //     return lenNow*3/2;
    // };

public:
    class GCNode;
    
private:
    static std::list<GCNode>        GlobalList;
    static std::atomic_flag         GlobalAtomic;
    static std::mutex               GlobalMutex;
    static std::condition_variable  GlobalCV;
    static bool                     GlobalExitFlag;
    static std::thread              GlobalGCThread;
    static size_t                   CountRun;
    static size_t                   CountWake;

    static std::atomic<int>         DebugStatus;
    static std::thread              DebugThread;
    static std::timed_mutex         DebugMutex;

public:
    template<typename T>
    static void AddNode(const ChainPtr<T> &cp);

    static void Run();
    static void Start();
    static void End();
};

}}

#endif
