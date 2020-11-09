#ifndef RTGC_DETAIL_CHAINCORE_HPP
#define RTGC_DETAIL_CHAINCORE_HPP

#include<cstddef>
#include<atomic>
#include<chrono>
#include<thread>

namespace RTGC { namespace detail {

template<typename T>
class ChainPtr;

//内层
template<typename T>
class ChainCore {
    friend class ChainPtr<T>;
    
    template<typename Tp, typename... _Args>
    friend ChainPtr<Tp> MakeChain(_Args&&... __args);
    
    friend class GCThread::GCNode;
private:
    T real;
    bool invalid = false;
    mutable std::atomic_flag mInnr = ATOMIC_FLAG_INIT; // 原子操作
    mutable const ChainPtr<T> *outr = nullptr; // 所有者结点

    inline void LockAtomic() {
        // 名称                         措施                        适用场景
        // BlockingWaitStrategy         加锁                        CPU资源紧缺，吞吐量和延迟并不重要的场景
        // BusySpinWaitStrategy         自旋                        通过不断重试，减少切换线程导致的系统调用，而降低延迟。推荐在线程绑定到固定的CPU的场景下使用
        // PhasedBackoffWaitStrategy    自旋 + yield + 自定义策略   CPU资源紧缺，吞吐量和延迟并不重要的场景
        // SleepingWaitStrategy         自旋 + yield + sleep        性能和CPU资源之间有很好的折中。延迟不均匀
        // TimeoutBlockingWaitStrategy  加锁，有超时限制            CPU资源紧缺，吞吐量和延迟并不重要的场景
        // YieldingWaitStrategy         自旋 + yield + 自旋         性能和CPU资源之间有很好的折中。延迟比较均匀

        // 此处使用YieldingWaitStrategy策略
        using namespace std::chrono_literals; // C++14
        if(!mInnr.test_and_set())
            return;
        std::this_thread::sleep_for(1ns);
        while(mInnr.test_and_set());
    }
    
    inline void UnlockAtomic() {
        mInnr.clear();
    }

    [[gnu::always_inline]] void Invalidate() { // 被动调用，此时this已上锁，且real需更新链
        invalid = true;
        if constexpr(decltype(haveInvalidate<T>(0))::value) {
            real.Invalidate();
        }
    }
    [[gnu::always_inline]] void TryValidate() { // 被动调用，此时this已上锁，且real需更新链
        while(mInnr.test_and_set()); //得到mInnr
        if(!outr->invalid) {
            invalid = false;
        } else if(!outr->inext->invalid) {
            invalid = false;
            outr = outr->inext;
        }
        mInnr.clear();
        if constexpr(decltype(haveTryValidate<T>(0))::value) {
            real.TryValidate(invalid);
        }
    }
    template<class... Args>
    ChainCore(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
