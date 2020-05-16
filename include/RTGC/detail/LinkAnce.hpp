#ifndef RTGC_DETAIL_LINKANCE_HPP
#define RTGC_DETAIL_LINKANCE_HPP

#include <string>

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






















// template<typename T>
// constexpr bool RTGC_IsComplex() {
//     return true;
// };
// template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value || std::is_pointer<T>::value>::type>
// constexpr bool RTGC_IsComplex() {
//     return false;
// };

template<typename, typename = void>
struct RTGC_IsComplex : public std::true_type {};

template<typename T>
struct RTGC_IsComplex<T, typename std::enable_if_t<
        std::is_fundamental<T>::value ||
        std::is_pointer<T>::value ||
        isCountPtr<T>::value ||
        isChainPtr<T>::value>> : public std::false_type {};




template<typename T>
auto RTGC_Auto(int) -> decltype(T::RTGC_Num, std::true_type());

template<typename T>
std::false_type RTGC_Auto();







template<typename U, typename... Args>
auto haveToTuple(int) -> decltype(std::declval<U>().ToTuple(), std::true_type());

template<typename U>
std::false_type haveToTuple(...);

// template<class T, class U>
// constexpr bool CanReferTo();

template<class T, class U, std::enable_if_t<!RTGC_IsComplex<T>::value, int> = 0>
constexpr bool CanReferTo() {
    if constexpr(std::is_fundamental<T>::value)
        return false;
    else if constexpr(isCountPtr<T>::value) {
        using V = typename isCountPtr<T>::type;
        if constexpr(std::is_same<V, U>::value)
            return true;
        else
            return CanReferTo<V, U>();
    }
    else if constexpr(isChainPtr<T>::value) {
        using V = typename isChainPtr<T>::type;
        if constexpr(std::is_same<V, U>::value)
            return true;
        else
            return CanReferTo<V, U>();
    }
    else
        static_assert(!std::is_pointer<T>::value);
}

template<class T, class U, std::enable_if_t<RTGC_IsComplex<T>::value && std::is_same<T, std::string>::value, int> = 0>
constexpr bool CanReferTo() {
    return false;
}


// template<class U, class MB>
// constexpr bool MakeCanReferToOne(MB &mb, bool &valid) {
//     if constexpr(std::is_same<MB, U>::value)
//         return true;
//     else
//         return CanReferTo<MB, U>();
//     // if constexpr(decltype(haveCanReferTo<MB>(0))::value) {
//     //     mb.CanReferTo(valid);
//     // }
// }

// template<class U, class Tuple, std::size_t... Is>
// constexpr bool MakeCanReferToImpl(const Tuple& t, bool &valid, std::index_sequence<Is...>) {
//     return (MakeCanReferToOne<U>(std::get<Is>(t)) || ...);
// }

// template<class U, class... Args>
// constexpr bool MakeCanReferTo(const std::tuple<Args...>& t) {
//     return MakeCanReferToImpl<U>(t, std::index_sequence_for<Args...>{});
// }

template<class T, class U>
constexpr bool MakeCanReferToOne() {
    if constexpr(std::is_same<T, U>::value)
        return true;
    else
        return CanReferTo<T, U>();
}

template<class Tuple, class U, std::size_t... Is>
constexpr bool MakeCanReferToImpl(std::index_sequence<Is...>) {
    return (MakeCanReferToOne<std::remove_reference_t<typename std::tuple_element<Is, Tuple>::type>, U>() || ...);
}

template<class Tuple, class U>
constexpr bool MakeCanReferTo() {
    return MakeCanReferToImpl<Tuple, U>(std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}
}}

#endif
