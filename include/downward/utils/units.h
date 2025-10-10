//
// Created by Thorsten Klößner on 10.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_UTILS_UNITS_H
#define DOWNWARD_UTILS_UNITS_H

#include "downward/concepts.h"

#include <ratio>

namespace downward {

template <typename T>
concept RatioType = SpecializationCTP<T, std::ratio>;

template <size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    char value[N];
};

template <size_t N>
StringLiteral(const char (&str)[N]) -> StringLiteral<N>;

template <ArithmeticType Rep, RatioType Period, StringLiteral Symbol>
class unit {
    Rep value;

public:
    using period = Period;
    using rep = Rep;

    explicit constexpr unit(Rep value)
        : value(value)
    {
    }

    constexpr auto count() const { return value; }

    friend std::ostream& operator<<(std::ostream& out, const unit& u)
    {
        return out << u.value << Symbol.value;
    }
};

namespace detail {
template <
    typename T,
    template <ArithmeticType, RatioType, StringLiteral> typename U>
struct SpecializationHelperUnit : std::false_type {};

template <
    ArithmeticType A,
    RatioType B,
    StringLiteral C,
    template <ArithmeticType, RatioType, StringLiteral> typename U>
struct SpecializationHelperUnit<U<A, B, C>, U> : std::true_type {};
} // namespace detail

template <
    typename T,
    template <ArithmeticType, RatioType, StringLiteral> typename U>
concept SpecializationUnit = detail::SpecializationHelperUnit<T, U>::value;

template <ArithmeticType Rep, RatioType Period, StringLiteral Symbol>
class ByteSize : public unit<Rep, Period, Symbol> {
    // Inherit constructor
    using ByteSize::unit::unit;
};

template <typename T>
concept ByteSizeType = SpecializationUnit<T, ByteSize>;

template <ByteSizeType To, class Rep, class Period, StringLiteral Symbol>
constexpr To bytesize_cast(const ByteSize<Rep, Period, Symbol>& dur) noexcept
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

using Bytes = ByteSize<std::intmax_t, std::ratio<1>, "B">;
using FBytes = ByteSize<long double, std::ratio<1>, "B">;

using Kilobytes = ByteSize<std::intmax_t, std::kilo, "KB">;
using Megabytes = ByteSize<std::intmax_t, std::mega, "MB">;
using Gigabytes = ByteSize<std::intmax_t, std::giga, "GB">;
using Terabytes = ByteSize<std::intmax_t, std::tera, "TB">;
using Petabytes = ByteSize<std::intmax_t, std::peta, "PB">;
using Exabytes = ByteSize<std::intmax_t, std::exa, "EB">;

using FKilobytes = ByteSize<long double, std::kilo, "KB">;
using FMegabytes = ByteSize<long double, std::mega, "MB">;
using FGigabytes = ByteSize<long double, std::giga, "GB">;
using FTerabytes = ByteSize<long double, std::tera, "TB">;
using FPetabytes = ByteSize<long double, std::peta, "PB">;
using FExabytes = ByteSize<long double, std::exa, "EB">;

using Kibibytes = ByteSize<std::intmax_t, std::ratio<1LL << 10>, "KiB">;
using Mebibytes = ByteSize<std::intmax_t, std::ratio<1LL << 20>, "MiB">;
using Gibibytes = ByteSize<std::intmax_t, std::ratio<1LL << 30>, "GiB">;
using Tebibytes = ByteSize<std::intmax_t, std::ratio<1LL << 40>, "TiB">;
using Pebibytes = ByteSize<std::intmax_t, std::ratio<1LL << 50>, "PiB">;
using Exbibytes = ByteSize<std::intmax_t, std::ratio<1LL << 60>, "EiB">;

using FKibibytes = ByteSize<long double, std::ratio<2LL << 10>, "KiB">;
using FMebibytes = ByteSize<long double, std::ratio<2LL << 20>, "MiB">;
using FGibibytes = ByteSize<long double, std::ratio<2LL << 30>, "GiB">;
using FTebibytes = ByteSize<long double, std::ratio<2LL << 40>, "TiB">;
using FPebibytes = ByteSize<long double, std::ratio<2LL << 50>, "PiB">;
using FExbibytes = ByteSize<long double, std::ratio<2LL << 60>, "EiB">;

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
