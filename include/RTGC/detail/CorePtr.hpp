#ifndef RTGC_DETAIL_COREPTR_HPP
#define RTGC_DETAIL_COREPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class ShellPtr;

//内层
template<typename T>
class CorePtr {
    friend class ShellPtr<T>;
    
    template<typename Tp, typename... _Args>
    friend ShellPtr<Tp> MakeShell(_Args&&... __args);
    
    T real;
    bool valid = true;
    ShellPtr<T> *outr = nullptr;//所有者结点

    [[gnu::always_inline]] void Invalidate() {//被动调用，此时this已上锁，且real需更新链
        valid = false;
        if constexpr(decltype(haveInvalidate<T>(0))::value) {
            real.Invalidate();
        }
    }
    [[gnu::always_inline]] void TryValidate() {//被动调用，此时this已上锁，且real需更新链
        if(outr->valid) {
            valid = true;
        } else for(auto i = outr->inext; i != nullptr; i = i->inext) {
            if(i->valid) {
                valid = true;
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
            real.TryValidate(valid);
        }
    }
    template<class... Args>
    CorePtr(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
