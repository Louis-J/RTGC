#ifndef RTGC_DETAIL_LINKANCE_HPP
#define RTGC_DETAIL_LINKANCE_HPP

namespace RTGC { namespace detail {

template<typename U, typename... Args>
auto haveInvalidate(int) -> decltype(std::declval<U>().Invalidate(), std::true_type());

template<typename U>
std::false_type haveInvalidate(...);


template<class MB>
constexpr void MakeInvalidateOne(MB &mb) {
    if constexpr(decltype(haveInvalidate<MB>(0))::value) {
        mb.Invalidate();
    }
}

template<class Tuple, std::size_t... Is>
constexpr void MakeInvalidateImpl(const Tuple& t, std::index_sequence<Is...>) {
    (MakeInvalidateOne(std::get<Is>(t)), ...);
}

template<class... Args>
constexpr void MakeInvalidate(const std::tuple<Args...>& t)
{
    MakeInvalidateImpl(t, std::index_sequence_for<Args...>{});
}


// template<typename U, typename... Args>
// auto LeafNode(int) -> decltype(std::declval<U>::LeafNode());

// template<typename U>
// std::false_type LeafNode(...);

// template<class MB>
// constexpr auto CheckLeafNodeOne(MB &mb) -> LeafNode<MB>::LeafNode() {}

// template<class Tuple, std::size_t... Is>
// constexpr auto CheckLeafNodeImpl(const Tuple& t, std::index_sequence<Is...>) {
//     return (CheckLeafNodeOne(std::get<Is>(t)), ...);
// }

// template<class... Args>
// constexpr auto CheckLeafNode(const std::tuple<Args...>& t)
// {
//     CheckLeafNodeImpl(t, n, o, std::index_sequence_for<Args...>{});
// }



template<typename U, typename... Args>
auto haveTryValidate(bool tmp) -> decltype(std::declval<U>().TryValidate(tmp), std::true_type());

template<typename U>
std::false_type haveTryValidate(...);

template<class MB>
constexpr void MakeTryValidateOne(MB &mb, bool &valid) {
    if constexpr(decltype(haveTryValidate<MB>(0))::value) {
        mb.TryValidate(valid);
    }
}

template<class Tuple, std::size_t... Is>
constexpr void MakeTryValidateImpl(const Tuple& t, bool &valid, std::index_sequence<Is...>) {
    (MakeTryValidateOne(std::get<Is>(t), valid), ...);
}

template<class... Args>
constexpr void MakeTryValidate(bool &valid, const std::tuple<Args...>& t)
{
    MakeTryValidateImpl(t, valid, std::index_sequence_for<Args...>{});
}



}}

#endif
