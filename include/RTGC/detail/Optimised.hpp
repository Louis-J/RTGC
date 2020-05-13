#ifndef RTGC_DETAIL_OPTIMISED_HPP
#define RTGC_DETAIL_OPTIMISED_HPP

#include<cstddef>
#include<atomic>

namespace RTGC { namespace detail {


template<class MB>
constexpr void MakeOptimisedInvalidateOne(MB &mb) {
    if constexpr(isShellPtr<MB>::value) {
        mb.IN2();
    }
}
template<class Tuple, std::size_t... Is>
constexpr void MakeOptimisedInvalidateImpl(const Tuple& t, std::index_sequence<Is...>) {
    (MakeOptimisedInvalidateOne(std::get<Is>(t)), ...);
}

template<class MB>
constexpr void MakeOptimisedTryValidateOne(MB &mb) {
    if constexpr(isShellPtr<MB>::value) {
        mb.TR2();
    }
}
template<class Tuple, std::size_t... Is>
constexpr void MakeOptimisedTryValidateImpl(const Tuple& t, std::index_sequence<Is...>) {
    (MakeOptimisedTryValidateOne(std::get<Is>(t)), ...);
}

template<class... Args>
constexpr void MakeOptimisedDes(Args&&... args) {
    MakeOptimisedInvalidateImpl(std::tuple<Args&...>{args... }, std::index_sequence_for<Args...>{});
    MakeOptimisedTryValidateImpl(std::tuple<Args&...>{args... }, std::index_sequence_for<Args...>{});
}

}}

#endif
