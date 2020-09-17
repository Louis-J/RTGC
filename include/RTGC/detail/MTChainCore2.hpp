#ifndef RTGC_DETAIL_MTCHAINCORE2_HPP
#define RTGC_DETAIL_MTCHAINCORE2_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class MTChainPtr2;

//内层
template<typename T>
class MTChainCore2 {
    friend class MTChainPtr2<T>;
    
    template<typename Tp, typename... _Args>
    friend MTChainPtr2<Tp> MakeChain(_Args&&... __args);
    
    T real;
    size_t invalid = 0;// 0 为存活, 非0为将亡时删除的起始标记，用来区别不同线程
    std::atomic_flag mInnr = ATOMIC_FLAG_INIT;
    MTChainPtr2<T> *outr = nullptr;//所有者结点

    [[gnu::always_inline]] void MTLockSub() {}
    [[gnu::always_inline]] void MTInvalidate(size_t tag) {//被动调用，此时outr已上锁, this已上锁，且real需更新链
        invalid = tag;
        if constexpr(decltype(haveMTInvalidate<T>(0))::value) {
            real.MTLockSub();
            mInnr.clear();
            real.MTInvalidate(tag);
        }
    }
    [[gnu::always_inline]] void MTTryLinkAnother(size_t tagOri, size_t tagDes) {
    }
    [[gnu::always_inline]] void MTTryValidate(size_t tag) {//被动调用，此时outr已上锁, this已上锁，且real需更新链
        if(outr->invalid == 0) {// 由已存活的outr调用
            invalid = 0;
        } else for(auto i = outr->inext; i != nullptr; i = i->inext) {
            if(!i->invalid) {
                invalid = false;
                auto ip = i->ipriv, in = i->inext;
                ip->inext = in;
                if(in != nullptr) {
                    in->ipriv = ip;
                }
                i->ipriv = nullptr;
                i->inext = outr;
                outr->ipriv = i;
                outr = i;
                break;
            }
        }
        if constexpr(decltype(haveMTTryValidate<T>(0))::value) {
            real.TryValidate(tag, invalid);
        }
    }
    template<class... Args>
    MTChainCore2(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
