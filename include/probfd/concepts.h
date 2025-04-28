//
// Created by Thorsten Klößner on 30.01.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_CONCEPTS_H
#define PROBFD_CONCEPTS_H

#include "probfd/type_traits.h"

#include "downward/concepts.h"

namespace probfd {

/// This concept is true if the variable specialization enable_pass_by_value<T>
/// has been defined as true.
template <typename T>
concept PassedByValue = enable_pass_by_value<T>;

} // namespace probfd

#endif // PROBFD_CONCEPTS_H
