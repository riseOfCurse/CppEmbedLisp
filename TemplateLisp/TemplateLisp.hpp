#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

// isSameTemplateValue
// template a -> template a -> Bool
template <template <auto...> typename _T, template <auto...> typename _U>
struct isSameTemplateValue {
    static constexpr bool value = false;
};

template <template <auto...> typename _T>
struct isSameTemplateValue<_T, _T> {
    static constexpr bool value = true;
};
template <template <typename...> typename _T, template <typename...> typename _U>
struct isSameTemplateType {
    static constexpr bool value = false;
};

template <template <typename...> typename _T>
struct isSameTemplateType<_T, _T> {
    static constexpr bool value = true;
};
template<template <auto...> typename T, template <auto...> typename U>
constexpr bool isSameTemplate(){
    return isSameTemplateValue<T, T>::value;
}
template<template <typename...> typename T, template <typename...> typename U>
constexpr bool isSameTemplate(){
    return isSameTemplateType<T, T>::value;
}

// hasSameTemplate
// a -> ... -> Bool
template <typename... Ts>
struct hasSameTemplate {
    static constexpr bool value = true;
};
template <
    template <typename...> typename T, typename... TArgs,
    template <typename...> typename U, typename... UArgs,
    typename... Rs>
struct hasSameTemplate<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool value = isSameTemplate<T, U>() && hasSameTemplate<U<UArgs...>, Rs...>::value;
};
template <
    template <auto...> typename T, auto... TArgs,
    template <auto...> typename U, auto... UArgs,
    typename... Rs>
struct hasSameTemplate<T<TArgs...>, U<UArgs...>, Rs...> {
    static constexpr bool value = isSameTemplate<T, U>() && hasSameTemplate<U<UArgs...>, Rs...>::value;
};

template <typename T>
struct Data {
    using dataType = T;
    T value;
    constexpr Data(T _value) : value(_value) {}
    constexpr operator T() const { return value; }
    constexpr T operator()() const { return value; }
    constexpr Data() = default;
    constexpr ~Data() = default;
};

template <typename T, size_t N>
struct Data<T[N]> {
    using dataType = T[N];
    static constexpr size_t dataLength = N;
    T value[N]{};
    constexpr Data(const T *_value) {
        std::copy(_value, _value + N, value);
    }
    constexpr operator const T *() const { return value; }
    constexpr const T *operator()() const { return value; }
    constexpr T operator[](const size_t n) { return value[n]; }
    constexpr Data() = default;
    constexpr ~Data() = default;
};

template <typename T, size_t N>
Data(const T (&)[N]) -> Data<T[N]>;

template <size_t N>
Data(const char (&)[N]) -> Data<char[N - 1]>;

template <typename T>
Data(T) -> Data<T>;

template <typename T>
concept IsCppCharArray = std::same_as<typename T::dataType, char[T::dataLength]>;

template <typename T>
concept IsCppNumber = (std::integral<typename T::dataType> || std::floating_point<typename T::dataType>)&&!std::same_as<typename T::dataType, char> && !std::same_as<typename T::dataType, bool>;

template <typename T>
concept IsCppChar = std::same_as<typename T::dataType, char>;

template <typename T>
concept IsCppBool = std::same_as<typename T::dataType, bool>;

template <Data x> requires(IsCppNumber<decltype(x)>) 
struct Number {};

template<size_t x>
using Index = Number<Data<size_t>(x)>;

template <Data x> requires(IsCppChar<decltype(x)>) 
struct Char {};

template <Data x> requires(IsCppBool<decltype(x)>) 
struct Bool {};

template <Data x> requires(IsCppCharArray<decltype(x)>) 
struct String {};

