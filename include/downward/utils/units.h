//
// Created by Thorsten Klößner on 10.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_UTILS_UNITS_H
#define DOWNWARD_UTILS_UNITS_H

#include "downward/concepts.h"

#include <numeric>
#include <ratio>

namespace downward {
template <typename T>
concept RatioType = SpecializationCTP<T, std::ratio>;

template <size_t N>
struct StringLiteral {
    consteval StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

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

    char value[N];
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

using kibi = std::ratio<2LL << 10>;
using mebi = std::ratio<2LL << 20>;
using gibi = std::ratio<2LL << 30>;
using tebi = std::ratio<2LL << 40>;
using pebi = std::ratio<2LL << 50>;
using exbi = std::ratio<2LL << 60>;

template <RatioType Period>
struct BytesSymbolStruct {
    static_assert(false, "Undefined symbol");
};

template <>
struct BytesSymbolStruct<std::ratio<1>> {
    static constexpr StringLiteral value = "B";
};

template <>
struct BytesSymbolStruct<std::kilo> {
    static constexpr StringLiteral value = "KB";
};

template <>
struct BytesSymbolStruct<std::mega> {
    static constexpr StringLiteral value = "MB";
};

template <>
struct BytesSymbolStruct<std::giga> {
    static constexpr StringLiteral value = "GB";
};

template <>
struct BytesSymbolStruct<std::tera> {
    static constexpr StringLiteral value = "TB";
};

template <>
struct BytesSymbolStruct<std::peta> {
    static constexpr StringLiteral value = "PB";
};

template <>
struct BytesSymbolStruct<std::exa> {
    static constexpr StringLiteral value = "EB";
};

template <>
struct BytesSymbolStruct<kibi> {
    static constexpr StringLiteral value = "KiB";
};

template <>
struct BytesSymbolStruct<mebi> {
    static constexpr StringLiteral value = "MiB";
};

template <>
struct BytesSymbolStruct<gibi> {
    static constexpr StringLiteral value = "GiB";
};

template <>
struct BytesSymbolStruct<tebi> {
    static constexpr StringLiteral value = "TiB";
};

template <>
struct BytesSymbolStruct<pebi> {
    static constexpr StringLiteral value = "PiB";
};

template <>
struct BytesSymbolStruct<exbi> {
    static constexpr StringLiteral value = "EiB";
};

template <RatioType Period>
inline constexpr auto BytesSymbol = BytesSymbolStruct<Period>::value;

template <ArithmeticType Rep, RatioType Period>
class unit {
    Rep value;

public:
    using period = Period;
    using rep = Rep;
    static constexpr auto symbol = BytesSymbol<Period>;

    explicit constexpr unit(Rep value)
        : value(value)
    {
    }

    constexpr auto count() const { return value; }

    friend std::ostream& operator<<(std::ostream& out, const unit& u)
    {
        return out << u.value << symbol.value;
    }
};

namespace detail {
template <typename T, template <ArithmeticType, RatioType> typename U>
struct SpecializationHelperUnit : std::false_type {};

template <
    ArithmeticType A,
    RatioType B,
    template <ArithmeticType, RatioType> typename U>
struct SpecializationHelperUnit<U<A, B>, U> : std::true_type {};
} // namespace detail

template <typename T, template <ArithmeticType, RatioType> typename U>
concept SpecializationUnit = detail::SpecializationHelperUnit<T, U>::value;

template <ArithmeticType Rep, RatioType Period>
class ByteSize : public unit<Rep, Period> {
    // Inherit constructor
    using ByteSize::unit::unit;
};
} // namespace downward

template <
    downward::ArithmeticType Rep1,
    downward::RatioType Period1,
    downward::ArithmeticType Rep2,
    downward::RatioType Period2>
struct std::common_type<
    downward::ByteSize<Rep1, Period1>,
    downward::ByteSize<Rep2, Period2>> {
    using type = downward::ByteSize<
        std::common_type_t<Rep1, Rep2>,
        std::ratio<
            std::gcd(Period1::num, Period2::num),
            std::lcm(Period1::den, Period2::den)>>;
};

template <downward::ArithmeticType Rep, downward::RatioType Period>
struct std::formatter<downward::ByteSize<Rep, Period>> {
    std::formatter<Rep> underlying_;

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class FmtContext>
    typename FmtContext::iterator
    format(const downward::ByteSize<Rep, Period>& size, FmtContext& ctx) const
    {
        return std::format_to(
            ctx.out(),
            downward::concat("{}", downward::BytesSymbol<Period>.value).value,
            size.count());
    }
};

namespace downward {

template <class Rep1, class Period1, class Rep2, class Period2>
std::common_type_t<ByteSize<Rep1, Period1>, ByteSize<Rep2, Period2>> constexpr
operator+(
    const ByteSize<Rep1, Period1>& lhs,
    const ByteSize<Rep2, Period2>& rhs)
{
    using CD =
        std::common_type_t<ByteSize<Rep1, Period1>, ByteSize<Rep2, Period2>>;
    return CD(CD(lhs).count() + CD(rhs).count());
}

template <class Rep1, class Period1, class Rep2, class Period2>
std::common_type_t<ByteSize<Rep1, Period1>, ByteSize<Rep2, Period2>> constexpr
operator-(
    const ByteSize<Rep1, Period1>& lhs,
    const ByteSize<Rep2, Period2>& rhs)
{
    using CD =
        std::common_type_t<ByteSize<Rep1, Period1>, ByteSize<Rep2, Period2>>;
    return CD(CD(lhs).count() - CD(rhs).count());
}

template <class Rep1, class Rep2, class Period>
std::common_type_t<std::common_type_t<Rep1, Rep2>, Period> constexpr
operator*(const ByteSize<Rep1, Period>& lhs, const Rep2& rhs)
{
    using CD = std::common_type_t<std::common_type_t<Rep1, Rep2>, Period>;
    return CD(CD(lhs).count() * rhs);
}

template <class Rep1, class Rep2, class Period>
std::common_type_t<std::common_type_t<Rep1, Rep2>, Period> constexpr
operator*(const Rep1& lhs, const ByteSize<Rep2, Period>& rhs)
{
    using CD = std::common_type_t<std::common_type_t<Rep1, Rep2>, Period>;
    return CD(lhs * CD(rhs).count());
}

template <typename T>
concept ByteSizeType = SpecializationUnit<T, ByteSize>;

template <ByteSizeType To, class Rep, class Period>
constexpr To bytesize_cast(const ByteSize<Rep, Period>& dur) noexcept
{
    // convert duration to another duration; truncate
    using CF = std::ratio_divide<Period, typename To::period>;

    using ToRep = typename To::rep;
    using CR = std::common_type_t<ToRep, Rep, intmax_t>;

    constexpr bool num_is_one = CF::num == 1;
    constexpr bool den_is_one = CF::den == 1;

    if constexpr (den_is_one) {
        if constexpr (num_is_one) {
            return static_cast<To>(static_cast<ToRep>(dur.count()));
        } else {
            return static_cast<To>(static_cast<ToRep>(
                static_cast<CR>(dur.count()) * static_cast<CR>(CF::num)));
        }
    } else {
        if constexpr (num_is_one) {
            return static_cast<To>(static_cast<ToRep>(
                static_cast<CR>(dur.count()) / static_cast<CR>(CF::den)));
        } else {
            return static_cast<To>(static_cast<ToRep>(
                static_cast<CR>(dur.count()) * static_cast<CR>(CF::num) /
                static_cast<CR>(CF::den)));
        }
    }
}

using Bytes = ByteSize<std::intmax_t, std::ratio<1>>;
using FBytes = ByteSize<long double, std::ratio<1>>;

using Kilobytes = ByteSize<std::intmax_t, std::kilo>;
using Megabytes = ByteSize<std::intmax_t, std::mega>;
using Gigabytes = ByteSize<std::intmax_t, std::giga>;
using Terabytes = ByteSize<std::intmax_t, std::tera>;
using Petabytes = ByteSize<std::intmax_t, std::peta>;
using Exabytes = ByteSize<std::intmax_t, std::exa>;

using FKilobytes = ByteSize<long double, std::kilo>;
using FMegabytes = ByteSize<long double, std::mega>;
using FGigabytes = ByteSize<long double, std::giga>;
using FTerabytes = ByteSize<long double, std::tera>;
using FPetabytes = ByteSize<long double, std::peta>;
using FExabytes = ByteSize<long double, std::exa>;

using Kibibytes = ByteSize<std::intmax_t, kibi>;
using Mebibytes = ByteSize<std::intmax_t, mebi>;
using Gibibytes = ByteSize<std::intmax_t, gibi>;
using Tebibytes = ByteSize<std::intmax_t, tebi>;
using Pebibytes = ByteSize<std::intmax_t, pebi>;
using Exbibytes = ByteSize<std::intmax_t, exbi>;

using FKibibytes = ByteSize<long double, kibi>;
using FMebibytes = ByteSize<long double, mebi>;
using FGibibytes = ByteSize<long double, gibi>;
using FTebibytes = ByteSize<long double, tebi>;
using FPebibytes = ByteSize<long double, pebi>;
using FExbibytes = ByteSize<long double, exbi>;

namespace bytes_literals {

consteval Bytes operator""_B(unsigned long long val) noexcept
{
    return Bytes(val);
}

consteval FBytes operator""_B(long double val) noexcept
{
    return FBytes(val);
}

consteval Kilobytes operator""_KB(unsigned long long val) noexcept
{
    return Kilobytes(val);
}

consteval FKilobytes operator""_KB(long double val) noexcept
{
    return FKilobytes(val);
}

consteval Kibibytes operator""_KiB(unsigned long long val) noexcept
{
    return Kibibytes(val);
}

consteval FKibibytes operator""_KiB(long double val) noexcept
{
    return FKibibytes(val);
}

consteval Megabytes operator""_MB(unsigned long long val) noexcept
{
    return Megabytes(val);
}

consteval FMegabytes operator""_MB(long double val) noexcept
{
    return FMegabytes(val);
}

consteval Mebibytes operator""_MiB(unsigned long long val) noexcept
{
    return Mebibytes(val);
}

consteval FMebibytes operator""_MiB(long double val) noexcept
{
    return FMebibytes(val);
}

consteval Gigabytes operator""_GB(unsigned long long val) noexcept
{
    return Gigabytes(val);
}

consteval FGigabytes operator""_GB(long double val) noexcept
{
    return FGigabytes(val);
}

consteval Gibibytes operator""_GiB(unsigned long long val) noexcept
{
    return Gibibytes(val);
}

consteval FGibibytes operator""_GiB(long double val) noexcept
{
    return FGibibytes(val);
}

consteval Terabytes operator""_TB(unsigned long long val) noexcept
{
    return Terabytes(val);
}

consteval FTerabytes operator""_TB(long double val) noexcept
{
    return FTerabytes(val);
}

consteval Tebibytes operator""_TiB(unsigned long long val) noexcept
{
    return Tebibytes(val);
}

consteval FTebibytes operator""_TiB(long double val) noexcept
{
    return FTebibytes(val);
}

consteval Petabytes operator""_PB(unsigned long long val) noexcept
{
    return Petabytes(val);
}

consteval FPetabytes operator""_PB(long double val) noexcept
{
    return FPetabytes(val);
}

consteval Pebibytes operator""_PiB(unsigned long long val) noexcept
{
    return Pebibytes(val);
}

consteval FPebibytes operator""_PiB(long double val) noexcept
{
    return FPebibytes(val);
}

consteval Exabytes operator""_EB(unsigned long long val) noexcept
{
    return Exabytes(val);
}

consteval FExabytes operator""_EB(long double val) noexcept
{
    return FExabytes(val);
}

consteval Exbibytes operator""_EiB(unsigned long long val) noexcept
{
    return Exbibytes(val);
}

consteval FExbibytes operator""_EiB(long double val) noexcept
{
    return FExbibytes(val);
}

} // namespace bytes_literals

} // namespace downward

#endif
