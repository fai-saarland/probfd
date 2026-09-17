//
// Created by Thorsten Klößner on 15.09.2026.
// Copyright (c) 2026 ProbFD contributors.
//

#ifndef UTILS_VALIDATION_H
#define UTILS_VALIDATION_H

#include <concepts>
#include <format>
#include <stdexcept>
#include <string_view>

namespace downward::utils {

/// Constructor tag for parameter-validating vs. non-validating constructors.
struct validation_tag {};

template <std::integral T1, std::integral T2>
void validate_param_geq(std::string_view arg_name, T1 x, T2 lb)
{
    if (std::cmp_less(x, lb)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not >= {}",
                arg_name,
                x,
                lb));
    }
}

template <typename T1, typename T2>
void validate_param_geq(std::string_view arg_name, T1 x, T2 lb)
{
    if (x < lb) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not >= {}",
                arg_name,
                x,
                lb));
    }
}

template <std::integral T1, std::integral T2>
void validate_param_leq(std::string_view arg_name, T1 x, T2 lb)
{
    if (std::cmp_greater(x, lb)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not <= {}",
                arg_name,
                x,
                lb));
    }
}

template <typename T1, typename T2>
void validate_param_leq(std::string_view arg_name, T1 x, T2 lb)
{
    if (x > lb) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not <= {}",
                arg_name,
                x,
                lb));
    }
}

template <std::integral T1, std::integral T2>
void validate_param_lt(std::string_view arg_name, T1 x, T2 lb)
{
    if (std::cmp_greater_equal(x, lb)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not < {}",
                arg_name,
                x,
                lb));
    }
}

template <typename T1, typename T2>
void validate_param_lt(std::string_view arg_name, T1 x, T2 lb)
{
    if (x >= lb) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not < {}",
                arg_name,
                x,
                lb));
    }
}

template <std::integral T1, std::integral T2>
void validate_param_gt(std::string_view arg_name, T1 x, T2 lb)
{
    if (std::cmp_less_equal(x, lb)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not > {}",
                arg_name,
                x,
                lb));
    }
}

template <typename T1, typename T2>
void validate_param_gt(std::string_view arg_name, T1 x, T2 lb)
{
    if (x <= lb) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not > {}",
                arg_name,
                x,
                lb));
    }
}

template <std::integral T1, std::integral T2>
void validate_param_neq(std::string_view arg_name, T1 x, T2 lb)
{
    if (std::cmp_equal(x, lb)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not != {}",
                arg_name,
                x,
                lb));
    }
}

template <typename T1, typename T2>
void validate_param_neq(std::string_view arg_name, T1 x, T2 lb)
{
    if (x == lb) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not != {}",
                arg_name,
                x,
                lb));
    }
}

template <std::integral T1, std::integral T2, std::integral T3>
void validate_param_in_range(std::string_view arg_name, T1 x, T2 lb, T3 ub)
{
    if (std::cmp_less(x, lb) || std::cmp_greater(x, ub)) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not in range [{}, {}]",
                arg_name,
                x,
                lb,
                ub));
    }
}

template <typename T1, typename T2, typename T3>
void validate_param_in_range(std::string_view arg_name, T1 x, T2 lb, T3 ub)
{
    if (x < lb || x > ub) {
        throw std::out_of_range(
            std::format(
                "Argument {} with value {} is not in range [{}, {}]",
                arg_name,
                x,
                lb,
                ub));
    }
}

template <typename T>
void validate_param_non_negative(std::string_view arg_name, T x)
{
    validate_param_geq(arg_name, x, 0);
}

template <typename T>
void validate_param_non_positive(std::string_view arg_name, T x)
{
    validate_param_leq(arg_name, x, 0);
}

template <typename T>
void validate_param_positive(std::string_view arg_name, T x)
{
    validate_param_gt(arg_name, x, 0);
}

template <typename T>
void validate_param_negative(std::string_view arg_name, T x)
{
    validate_param_lt(arg_name, x, 0);
}

template <typename T>
void validate_param_non_zero(std::string_view arg_name, T x)
{
    validate_param_neq(arg_name, x, 0);
}

template <typename T>
void validate_param_non_null(std::string_view arg_name, T x)
{
    if (x == nullptr) {
        throw std::invalid_argument(
            std::format("Argument {} is a nullptr.", arg_name));
    }
}

} // namespace downward::utils

#endif // UTILS_VALIDATION_H
