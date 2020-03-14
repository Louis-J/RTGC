#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"
#include"detail/ShellPtr.hpp"
#include"detail/LinkAnce.hpp"
#include"detail/CorePtr.hpp"

namespace RTGC {

template<typename T>
using CorePtr = detail::CorePtr<T>;

template<typename T>
using ShellPtr = detail::ShellPtr<T>;

}

#define CLASSLINK(cName, mNum)                                                                                  \
using thisT = cName;                                                                                            \
friend struct RTGC::detail::toTuple<thisT>;                                                                     \
constexpr inline void Invalidate() {                                                                            \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<2>());       \
    RTGC::detail::MakeInvalidate(tieTuple);                                                                     \
}                                                                                                               \
constexpr inline void TryValidate(bool &valid) {                                                                \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<2>());       \
    RTGC::detail::MakeTryValidate(valid, tieTuple);                                                             \
}

#endif
