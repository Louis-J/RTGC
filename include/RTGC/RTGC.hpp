#ifndef RTGC_HPP
#define RTGC_HPP

#include<type_traits>
#include<utility>
#include"detail/ClassMembers.hpp"
#include"detail/CorePtr.hpp"
#include"detail/ShellPtr.hpp"
#include"RootPtr.hpp"
#include"SnapPtr.hpp"

namespace RTGC {

template<typename T>
using CorePtr = detail::CorePtr<T>;

template<typename U>
static constexpr auto haveLinkAnce(int) -> decltype(U::LinkAnce, std::true_type());

template<typename U>
static constexpr std::false_type haveLinkAnce(...);



template<class MB>
constexpr void linkMemberOne(MB &mb, void *n, void *o) {
    static_assert(
        !isRootPtr<MB>::value,
        "====================> RTGC: cannot use root_ptr in a class!"
    );
    if constexpr(std::is_same<decltype(haveLinkAnce<MB>(0)), std::true_type>::value) {
        mb.LinkAnce(n, o);
    }
}

template<class Tuple, std::size_t... Is>
constexpr void linkMembersImpl(const Tuple& t, void *n, void *o, std::index_sequence<Is...>) {
    (linkMemberOne(std::get<Is>(t), n, o), ...);
}

template<class... Args>
constexpr void linkMembers(void *n, void *o, const std::tuple<Args...>& t)
{
    linkMembersImpl(t, n, o, std::index_sequence_for<Args...>{});
}

}

#define CLASSLINK(cName, mNum)                                                                                  \
using thisT = cName;                                                                                            \
friend class RTGC::detail::toTuple<thisT>;                                                                     \
public:                                                                                                         \
constexpr inline void LinkAnce(void *n, void *o = nullptr) {                                                    \
    auto &&tieTuple = RTGC::detail::toTuple<thisT>::tie_as_tuple(*this, RTGC::detail::const_size_t<mNum>());  \
    RTGC::linkMembers(n, o, tieTuple);                                                                          \
}

#endif
