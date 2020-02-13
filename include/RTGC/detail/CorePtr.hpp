#ifndef RTGC_DETAIL_COREPTR_HPP
#define RTGC_DETAIL_COREPTR_HPP

#include <cstddef>

namespace RTGC { namespace detail {

template<typename T>
class ShellPtr;

//内层
template<typename T>
class CorePtr {
    friend class ShellPtr<T>;
    
    ShellPtr<T> *outr = nullptr;//所有者结点
    T real;
    void LinkAnce(void *n, void *o) {
        if constexpr(decltype(haveLinkAnce<T>(0))::value) {
            real.LinkAnce(n, o);
        }
    }
public:
    template<class... Args>
    CorePtr(Args&&... args) :real(std::forward<Args>(args)...) {
        if constexpr(decltype(haveLinkInit<T>(0))::value) {
            real.LinkInit(nullptr);
        }
    }
};

}}

#endif
