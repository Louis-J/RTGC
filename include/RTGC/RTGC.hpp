#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"

#if RTGCSPUSE == 1
#include"detail/ShellPtr1.hpp"
#elif RTGCSPUSE == 2
#include"detail/ShellPtr2.hpp"
#else
#include"detail/ShellPtr.hpp"
#endif

#include"detail/LinkAnce.hpp"
#include"detail/CorePtr.hpp"

namespace RTGC {

template<typename T>
using ShellPtr = detail::ShellPtr<T>;

template<typename _Tp, typename... _Args>
inline ShellPtr<_Tp> MakeShell(_Args&&... __args) {
    return detail::MakeShell<_Tp>(std::forward<_Args>(__args)...);
}


}

#define CLASSLINK(cName, mNum)                                                                                  \
using thisT = cName;                                                                                            \
friend struct RTGC::detail::toTuple<thisT>;                                                                     \
[[gnu::always_inline]] constexpr inline void Invalidate() {                                                     \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<mNum>());    \
    RTGC::detail::MakeInvalidate(tieTuple);                                                                     \
}                                                                                                               \
[[gnu::always_inline]] constexpr inline void TryValidate(bool &valid) {                                         \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<mNum>());    \
    RTGC::detail::MakeTryValidate(valid, tieTuple);                                                             \
}

#endif
