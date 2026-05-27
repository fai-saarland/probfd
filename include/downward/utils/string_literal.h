//
// Created by Thorsten Klößner on 11.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_STRING_LITERAL_H
#define DOWNWARD_STRING_LITERAL_H

#include <array>
#include <compare>
#include <format>
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
        requires(N1 + N2 == N + 1)
    consteval StringLiteral(
        StringLiteral<N1> str1,
        StringLiteral<N2> str2,
        std::index_sequence<indices1...>,
        std::index_sequence<indices2...>)
        : value{str1[indices1]..., str2[indices2]...}
    {
    }

public:
    const std::array<char, N> value{};

    /// Char array constructor
    consteval StringLiteral(const char (&str)[N])
        : value(std::to_array(str))
    {
    }

    /// Char array constructor
    consteval StringLiteral(std::array<char, N> array)
        : value(std::move(array))
    {
    }

    /// Concatenation constructor
    template <size_t N1, size_t N2>
        requires(N1 + N2 == N + 1)
    consteval StringLiteral(StringLiteral<N1> str1, StringLiteral<N2> str2)
        : StringLiteral(
              str1,
              str2,
              std::make_index_sequence<N1 - 1>{},
              std::make_index_sequence<N2>{})
    {
    }

    consteval char operator[](size_t index) const { return value[index]; }

    consteval operator std::string_view() const
    {
        return std::string_view(value.data());
    }
};

template <std::size_t N1, std::size_t N2>
consteval StringLiteral<N1 + N2 - 1>
concat(StringLiteral<N1> str1, StringLiteral<N2> str2)
{
    return StringLiteral<N1 + N2 - 1>(str1, str2);
}

template <std::size_t N1, std::size_t N2>
consteval StringLiteral<N1 + N2 - 1>
concat(const char (&str1)[N1], const char (&str2)[N2])
{
    return StringLiteral<N1 + N2 - 1>(StringLiteral(str1), StringLiteral(str2));
}

template <std::size_t N>
consteval bool
operator==(const StringLiteral<N>& left, const StringLiteral<N>& right)
{
    return left.value == right.value;
}

template <std::size_t N>
consteval auto
operator<=>(const StringLiteral<N>& left, const StringLiteral<N>& right)
{
    return left.value == right.value;
}

template <size_t N>
StringLiteral(const char (&str)[N]) -> StringLiteral<N>;

namespace string_literals {
// String literal operator
// The string literal "abc"_sl is replaced by the call operator""<"abc">_sl(),
// which instantiates the StringLiteral template parameter A via the consteval
// constructor call.
// The length of the string is deduced via the deduction guide above.
template <StringLiteral A>
constexpr auto operator""_sl()
{
    return A;
}

} // namespace string_literals

} // namespace downward::utils

// Formatting support
template <std::size_t N>
struct std::formatter<downward::utils::StringLiteral<N>> {
    std::formatter<const char*> underlying_;

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class FmtContext>
    typename FmtContext::iterator
    format(const downward::utils::StringLiteral<N>& sl, FmtContext& ctx) const
    {
        return underlying_.format(sl.value.data(), ctx);
    }
};

#endif
