#ifndef RTGC_DETAIL_COUNTCORE_HPP
#define RTGC_DETAIL_COUNTCORE_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class CountPtr;

//内层
template<typename T>
class CountCore {
    friend class CountPtr<T>;
    
    template<typename Tp, typename... _Args>
    friend CountPtr<Tp> MakeCount(_Args&&... __args);
    
    size_t rc = 0;
    T real;

    template<class... Args>
    CountCore(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
