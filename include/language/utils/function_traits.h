//
// Created by Thorsten Klößner on 04.01.2026.
// Copyright (c) 2026 ProbFD contributors.
//

#ifndef LANGUAGE_UTILS_FUNCTION_TRAITS_H
#define LANGUAGE_UTILS_FUNCTION_TRAITS_H

#include <tuple>

namespace language::utils {

template <typename F>
struct FuncTraits {
    static_assert(
        sizeof(F) == 0,
        "Template parameter of FuncTraits is not a function pointer!");
};

template <typename R, typename... Args>
struct FuncTraits<R (*)(Args...)> {
    using ReturnType = R;

    template <std::size_t I>
    using ArgType = std::tuple_element_t<I, std::tuple<Args...>>;

    static constexpr std::size_t num_args = sizeof...(Args);
};

template <typename R, typename... Args>
struct FuncTraits<R (*const)(Args...)> : FuncTraits<R (*)(Args...)> {};

template <typename R, typename... Args>
struct FuncTraits<R (*const volatile)(Args...)> : FuncTraits<R (*)(Args...)> {};

template <typename R, typename... Args>
struct FuncTraits<R (*volatile)(Args...)> : FuncTraits<R (*)(Args...)> {};

template <typename T>
constexpr auto num_args = FuncTraits<T>::num_args;

template <typename T>
using ReturnType = typename FuncTraits<T>::ReturnType;

template <typename T, std::size_t I>
using ArgType = typename FuncTraits<T>::template ArgType<I>;

} // namespace language::utils

#endif // LANGUAGE_UTILS_FUNCTION_TRAITS_H
