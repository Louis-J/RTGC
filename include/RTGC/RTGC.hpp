#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"
#include"detail/CorePtr.hpp"
#include"detail/ShellPtr.hpp"
#include"detail/RootPtr.hpp"
#include"detail/SnapPtr.hpp"
#include"detail/LinkAnce.hpp"

namespace RTGC {

template<typename T>
using CorePtr = detail::CorePtr<T>;

template<typename T>
using RootPtr = detail::RootPtr<T>;

template<typename T>
using SnapPtr = detail::SnapPtr<T>;

template<typename T>
using ShellPtr = detail::ShellPtr<T>;

}

#define CLASSLINK(cName, mNum)                                                                                  \
using thisT = cName;                                                                                            \
friend struct RTGC::detail::toTuple<thisT>;                                                                      \
public:                                                                                                         \
constexpr inline void LinkAnce(void *n, void *o = nullptr) {                                                    \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<mNum>());    \
    RTGC::detail::linkMembers(n, o, tieTuple);                                                                  \
}

#endif
