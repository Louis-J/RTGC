#ifndef RTGC_DETAIL_SMARTERPTR_HPP
#define RTGC_DETAIL_SMARTERPTR_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {

template<typename T, bool MayCirRef, typename = void>
struct ChosePtr;

template<typename T, bool MayCirRef>
struct ChosePtr<T, MayCirRef, typename std::enable_if_t<MayCirRef>> {
    using type = ChainPtr<T>;
};

template<typename T, bool MayCirRef>
struct ChosePtr<T, MayCirRef, typename std::enable_if_t<!MayCirRef>> {
    using type = CountPtr<T>;
};

template<typename T>
using SmarterPtr = typename ChosePtr<T, T::RTGC_MayCirRef>::type;

}}


#endif
