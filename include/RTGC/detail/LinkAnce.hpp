#ifndef RTGC_DETAIL_LINKANCE_HPP
#define RTGC_DETAIL_LINKANCE_HPP

namespace RTGC { namespace detail {

template<typename U, typename... Args>
auto haveLinkAnce(int) -> decltype(std::declval<U>().LinkAnce(nullptr, nullptr), std::true_type());

template<typename U>
std::false_type haveLinkAnce(...);

template<typename U, typename... Args>
auto haveLinkInit(int) -> decltype(std::declval<U>().LinkInit(nullptr), std::true_type());

template<typename U>
std::false_type haveLinkInit(...);

template<class MB>
constexpr void MakeLinkInitOne(MB &mb, void *n) {
    if constexpr(isShellPtr<MB>::value) {
        mb.LinkInit(n);
    }
}

template<class Tuple, std::size_t... Is>
constexpr void MakeLinkInitImpl(const Tuple& t, void *n, std::index_sequence<Is...>) {
    (MakeLinkInitOne(std::get<Is>(t), n), ...);
}

template<class... Args>
constexpr void MakeLinkInit(void *n, const std::tuple<Args...>& t)
{
    MakeLinkInitImpl(t, n, std::index_sequence_for<Args...>{});
}


template<class MB>
constexpr void MakeLinkAnceOne(MB &mb, void *n, void *o) {
    if constexpr(decltype(haveLinkAnce<MB>(0))::value) {
        mb.LinkAnce(n, o);
    }
}

template<class Tuple, std::size_t... Is>
constexpr void MakeLinkAnceImpl(const Tuple& t, void *n, void *o, std::index_sequence<Is...>) {
    (MakeLinkAnceOne(std::get<Is>(t), n, o), ...);
}

template<class... Args>
constexpr void MakeLinkAnce(void *n, void *o, const std::tuple<Args...>& t)
{
    MakeLinkAnceImpl(t, n, o, std::index_sequence_for<Args...>{});
}

}}

#endif
