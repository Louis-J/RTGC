#ifndef RTGC_DETAIL_CHAINCORE_HPP
#define RTGC_DETAIL_CHAINCORE_HPP

#include<cstddef>
#include<atomic>

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
public:
    T real;
    bool invalid = false;
    mutable std::atomic_flag mInnr = ATOMIC_FLAG_INIT; // 原子操作
    mutable const ChainPtr<T> *outr = nullptr; // 所有者结点

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
