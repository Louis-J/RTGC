#ifndef RTGC_DETAIL_ChainCore_HPP
#define RTGC_DETAIL_ChainCore_HPP

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
    
    T real;
    bool invalid = false;
    mutable const ChainPtr<T> *outr = nullptr;//所有者结点

    [[gnu::always_inline]] void Invalidate() {//被动调用，此时this已上锁，且real需更新链
        invalid = true;
        if constexpr(decltype(haveInvalidate<T>(0))::value) {
            real.Invalidate();
        }
    }
    [[gnu::always_inline]] void TryValidate() {//被动调用，此时this已上锁，且real需更新链
        if(!outr->invalid) {
            invalid = false;
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
        if constexpr(decltype(haveTryValidate<T>(0))::value) {
            real.TryValidate(invalid);
        }
    }
    template<class... Args>
    ChainCore(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
