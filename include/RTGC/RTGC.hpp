#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"

#include"detail/ChainPtr.hpp"
#include"detail/CountPtr.hpp"
// #include"detail/MTChainPtr.hpp"
#include"detail/MTCountPtr.hpp"

#include"detail/LinkAnce.hpp"
#include"detail/SmarterPtr.hpp"

#include"detail/ChainCore.hpp"
#include"detail/CountCore.hpp"
// #include"detail/MTChainCore.hpp"
#include"detail/MTCountCore.hpp"
#include"detail/Optimised.hpp"

// 由于是侵入式的设计，因此不支持多继承的多态(无法检测多继承而不支持)。计划编写类似于Java继承规则的继承工具(保证继承的虚表最多只有一个，多态时不会改变内存地址)
// ChainPtr/MTChainPtr保证不会出现循环引用，CountPtr/MTCountPtr有可能有循环引用
// MT的智能指针仅保证，在不同线程不允许拥有同一个智能指针的引用的情况下，的内存安全。若不同线程拥有了对同一个智能指针的引用，则可能发生问题。
// MT的智能指针与shared_ptr有同级别的线程安全性，非MT的智能指针不保证线程安全。MTChainPtr相比于ChainPtr性能有可见的下降。

namespace RTGC {

// ChainPtr
template<typename T>
using ChainPtr = detail::ChainPtr<T>;

template<typename _Tp, typename... _Args>
inline ChainPtr<_Tp> MakeChain(_Args&&... __args) {
    return detail::MakeChain<_Tp>(std::forward<_Args>(__args)...);
}

// CountPtr
template<typename T>
using CountPtr = detail::CountPtr<T>;

template<typename _Tp, typename... _Args>
inline CountPtr<_Tp> MakeCount(_Args&&... __args) {
    return detail::MakeCount<_Tp>(std::forward<_Args>(__args)...);
}

template<typename T>
using MTCountPtr = detail::MTCountPtr<T>;

template<typename _Tp, typename... _Args>
inline MTCountPtr<_Tp> MakeMTCount(_Args&&... __args) {
    return detail::MakeMTCount<_Tp>(std::forward<_Args>(__args)...);
}

// SmarterPtr
template<typename T>
using SmarterPtr = detail::SmarterPtr<T>;

template<typename T, std::enable_if_t<T::RTGC_MayCirRef, int> = 0, typename... _Args>
inline ChainPtr<T> MakeSmarter(_Args&&... __args) {
    return detail::MakeChain<T>(std::forward<_Args>(__args)...);
}

template<typename T, std::enable_if_t<!T::RTGC_MayCirRef, int> = 0, typename... _Args>
inline CountPtr<T> MakeSmarter(_Args&&... __args) {
    return detail::MakeCount<T>(std::forward<_Args>(__args)...);
}

// OptimisedDestructionForChainPtr
template<class... Args>
constexpr void MakeOptimisedDes(Args&&... args) {
    detail::MakeOptimisedInvalidateImpl(std::tuple<Args&...>{args... }, std::index_sequence_for<Args...>{});
    detail::MakeOptimisedTryValidateImpl(std::tuple<Args&...>{args... }, std::index_sequence_for<Args...>{});
}

}

#define RTGC_AutoChainLink(cName, mNum)                                                                                        \
using thisT = cName;                                                                                                           \
using RTGC_Num = RTGC::detail::const_size_t<mNum>;                                                                             \
friend struct RTGC::detail::ToTuple<thisT>;                                                                                    \
[[gnu::always_inline]] constexpr inline void Invalidate() {                                                                    \
    auto &&tieTuple = RTGC::detail::ToTuple<thisT>::TieToTuple(*this, RTGC::detail::const_size_t<mNum>());                     \
    RTGC::detail::MakeInvalidate(tieTuple);                                                                                    \
}                                                                                                                              \
[[gnu::always_inline]] constexpr inline void TryValidate(bool &valid) {                                                        \
    auto &&tieTuple = RTGC::detail::ToTuple<thisT>::TieToTuple(*this, RTGC::detail::const_size_t<mNum>());                     \
    RTGC::detail::MakeTryValidate(valid, tieTuple);                                                                            \
}

#define RTGC_AutoCRDetectIn(cName, mNum)                                                                                       \
constexpr inline auto ToTuple() {                                                                                              \
    return RTGC::detail::ToTuple<thisT>::TieToTuple(*this, RTGC::detail::const_size_t<mNum>());                                \
}

#define RTGC_AutoCRDetectOut(cName, enable)                                                                                    \
namespace RTGC { namespace detail {                                                                                            \
template<class T, class U, std::enable_if_t<RTGC_IsComplex<T>::value && std::is_same<T, cName>::value, int> = 0>               \
constexpr bool CanReferTo() {                                                                                                  \
    using V = decltype(std::declval<cName>().ToTuple());                                                                       \
    return MakeCanReferTo<V, U>();                                                                                             \
}                                                                                                                              \
}}                                                                                                                             \
static_assert(RTGC::detail::CanReferTo<cName, cName>() == enable);

#endif
