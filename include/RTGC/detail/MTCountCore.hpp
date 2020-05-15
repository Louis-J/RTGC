#ifndef RTGC_DETAIL_MTCOUNTCORE_HPP
#define RTGC_DETAIL_MTCOUNTCORE_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T>
class MTCountPtr;

//内层
template<typename T>
class MTCountCore {
    friend class MTCountPtr<T>;
    
    template<typename Tp, typename... _Args>
    friend MTCountPtr<Tp> MakeCount(_Args&&... __args);
    
    std::atomic<size_t> rc = 0;
    T real;

    template<class... Args>
    MTCountCore(Args&&... args) :real(std::forward<Args>(args)...) {}
};

}}

#endif
