#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"

#include"detail/ShellPtr.hpp"
#include"detail/LinkAnce.hpp"
#include"detail/CorePtr.hpp"
#include"detail/Optimised.hpp"

namespace RTGC {

template<typename T>
using ShellPtr = detail::ShellPtr<T>;

template<typename _Tp, typename... _Args>
inline ShellPtr<_Tp> MakeShell(_Args&&... __args) {
    return detail::MakeShell<_Tp>(std::forward<_Args>(__args)...);
}

template<class... Args>
constexpr void MakeOptimisedDes2(Args&&... args) {
    return detail::MakeOptimisedDes(std::forward<Args>(args)...);
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
