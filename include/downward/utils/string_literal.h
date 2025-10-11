//
// Created by Thorsten Klößner on 11.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_STRING_LITERAL_H
#define DOWNWARD_STRING_LITERAL_H

#include <algorithm>
#include <utility>

namespace downward::utils {

/// Compile-Time String Literal
template <size_t N>
class StringLiteral {
    template <
        size_t N1,
        size_t N2,
        std::size_t... indices1,
        std::size_t... indices2>
        requires(N1 + N2 == N)
    consteval StringLiteral(
        StringLiteral<N1> str1,
        StringLiteral<N2> str2,
        std::index_sequence<indices1...>,
        std::index_sequence<indices2...>)
        : StringLiteral({str1[indices1]..., str2[indices2]..., '\0'})
    {
    }

public:
    char value[N];

    /// Char array constructor
    consteval StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    /// Concatenation constructor
    template <size_t N1, size_t N2>
        requires(N1 + N2 == N)
    consteval StringLiteral(StringLiteral<N1> str1, StringLiteral<N2> str2)
        : StringLiteral(
              str1,
              str2,
              std::make_index_sequence<N1 - 1>{},
              std::make_index_sequence<N2>{})
    {
    }

    consteval char operator[](size_t index) const { return value[index]; }
};

template <std::size_t N1, std::size_t N2>
consteval StringLiteral<N1 + N2>
concat(StringLiteral<N1> str1, StringLiteral<N2> str2)
{
    return StringLiteral<N1 + N2>(str1, str2);
}

template <std::size_t N1, std::size_t N2>
consteval StringLiteral<N1 + N2>
concat(const char (&str1)[N1], const char (&str2)[N2])
{
    return StringLiteral<N1 + N2>(StringLiteral(str1), StringLiteral(str2));
}

template <size_t N>
StringLiteral(const char (&str)[N]) -> StringLiteral<N>;

} // namespace downward::utils

#endif
