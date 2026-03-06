//
// Created by Thorsten Klößner on 31.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_UTILS_CONCEPTS_H
#define LANGUAGE_UTILS_CONCEPTS_H

#include <type_traits>

namespace language::utils {

template <template <bool...> typename T, bool... b>
concept instantiable = requires { typename T<b...>; };

template <template <bool...> typename T, bool... b>
struct suffix_instantiable_h : std::false_type {};

template <template <bool...> typename T, bool b, bool... tail>
struct suffix_instantiable_h<T, b, tail...>
    : std::bool_constant<
          instantiable<T, b, tail...> ||
          suffix_instantiable_h<T, tail...>::value> {};

template <template <bool...> typename T, bool... b>
concept suffix_instantiable = suffix_instantiable_h<T, b...>::value;

template <template <bool...> typename T, bool... b>
concept strict_suffix_instantiable =
    suffix_instantiable<T, b...> && !instantiable<T, b...>;

template <template <bool...> typename T, bool... b>
concept partial_specialization = !strict_suffix_instantiable<T, b...>;

}

#endif //LANGUAGE_UTILS_CONCEPTS_H
