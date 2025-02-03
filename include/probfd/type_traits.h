//
// Created by Thorsten Klößner on 03.02.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_TYPE_TRAITS_H
#define PROBFD_TYPE_TRAITS_H

#include <type_traits>

namespace probfd {

/// This template variable controls the type of ParamType<T>.
template <typename T>
constexpr bool enable_pass_by_value = false;

template <typename T>
    requires(std::is_scalar_v<T>)
constexpr bool enable_pass_by_value<T> = true;

}

#endif // PROBFD_TYPE_TRAITS_H
