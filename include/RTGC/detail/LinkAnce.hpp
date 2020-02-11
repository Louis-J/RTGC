#ifndef RTGC_DETAIL_LINKANCE_HPP
#define RTGC_DETAIL_LINKANCE_HPP

namespace RTGC { namespace detail {


template<class MB>
constexpr void linkMemberOne(MB &mb, void *n, void *o) {
    static_assert(
        !isRootPtr<MB>::value,
        "====================> RTGC: cannot use root_ptr in a class!"
    );
    if constexpr(decltype(haveLinkAnce<MB>(0))::value) {
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

}}

#endif
