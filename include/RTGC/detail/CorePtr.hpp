#ifndef RTGC_DETAIL_COREPTR_HPP
#define RTGC_DETAIL_COREPTR_HPP

namespace RTGC { namespace detail {

template<typename T>
class ShellPtr;

//内层
template<typename T>
class CorePtr {
    friend class ShellPtr<T>;
    
    ShellPtr<T> *outr = nullptr;//所有者结点
    T real;
    void LinkAnce(void *n, void *o = nullptr){
        if constexpr(std::is_same<decltype(haveLinkAnce<T>(0)), std::true_type>::value) {
            real.LinkAnce(n, o);
        }
    }
public:
    template<class... Args>
    CorePtr(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
