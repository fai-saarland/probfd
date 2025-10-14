//
// Created by Thorsten Klößner on 10.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_UTILS_BYTE_UNITS_H
#define DOWNWARD_UTILS_BYTE_UNITS_H

#include "downward/utils/units.h"

namespace downward::utils {

/// Represents a size in bytes
template <ArithmeticType Rep, RatioType Period>
struct ByteSize : public unit<Rep, Period> {
    // Inherit constructor
    using ByteSize::unit::unit;
};

/// Size in bytes represented using arithmetic type T.
template <ArithmeticType T>
using GBytes = ByteSize<T, std::ratio<1>>;

/// Size in kilobytes represented using arithmetic type T.
template <ArithmeticType T>
using GKilobytes = ByteSize<T, std::kilo>;

/// Size in megabytes represented using arithmetic type T.
template <ArithmeticType T>
using GMegabytes = ByteSize<T, std::mega>;

/// Size in gigabytes represented using arithmetic type T.
template <ArithmeticType T>
using GGigabytes = ByteSize<T, std::giga>;

/// Size in terabytes represented using arithmetic type T.
template <ArithmeticType T>
using GTerabytes = ByteSize<T, std::tera>;

/// Size in petabytes represented using arithmetic type T.
template <ArithmeticType T>
using GPetabytes = ByteSize<T, std::peta>;

/// Size in exabytes represented using arithmetic type T.
template <ArithmeticType T>
using GExabytes = ByteSize<T, std::exa>;

/// Size in kibibytes represented using arithmetic type T.
template <ArithmeticType T>
using GKibibytes = ByteSize<T, kibi>;

/// Size in mebibytes represented using arithmetic type T.
template <ArithmeticType T>
using GMebibytes = ByteSize<T, mebi>;

/// Size in gibibytes represented using arithmetic type T.
template <ArithmeticType T>
using GGibibytes = ByteSize<T, gibi>;

/// Size in tebibytes represented using arithmetic type T.
template <ArithmeticType T>
using GTebibytes = ByteSize<T, tebi>;

/// Size in pebibytes represented using arithmetic type T.
template <ArithmeticType T>
using GPebibytes = ByteSize<T, pebi>;

/// Size in exbibytes represented using arithmetic type T.
template <ArithmeticType T>
using GExbibytes = ByteSize<T, exbi>;

/// Size in bytes represented using std::intmax_t.
using Bytes = GBytes<std::intmax_t>;

/// Size in kilobytes represented using std::intmax_t.
using Kilobytes = GKilobytes<std::intmax_t>;

/// Size in megabytes represented using std::intmax_t.
using Megabytes = GMegabytes<std::intmax_t>;

/// Size in gigabytes represented using std::intmax_t.
using Gigabytes = GGigabytes<std::intmax_t>;

/// Size in terabytes represented using std::intmax_t.
using Terabytes = GTerabytes<std::intmax_t>;

/// Size in petabytes represented using std::intmax_t.
using Petabytes = GPetabytes<std::intmax_t>;

/// Size in exabytes represented using std::intmax_t.
using Exabytes = GExabytes<std::intmax_t>;

/// Size in kibibytes represented using std::intmax_t.
using Kibibytes = GKibibytes<std::intmax_t>;

/// Size in mebibytes represented using std::intmax_t.
using Mebibytes = GMebibytes<std::intmax_t>;

/// Size in gibibytes represented using std::intmax_t.
using Gibibytes = GGibibytes<std::intmax_t>;

/// Size in tebibytes represented using std::intmax_t.
using Tebibytes = GTebibytes<std::intmax_t>;

/// Size in pebibytes represented using std::intmax_t.
using Pebibytes = GPebibytes<std::intmax_t>;

/// Size in exbibytes represented using std::intmax_t.
using Exbibytes = GExbibytes<std::intmax_t>;

/// Size in bytes represented using long double.
using FBytes = GBytes<long double>;

/// Size in kilobytes represented using long double.
using FKilobytes = GKilobytes<long double>;

/// Size in megabytes represented using long double.
using FMegabytes = GMegabytes<long double>;

/// Size in gigabytes represented using long double.
using FGigabytes = GGigabytes<long double>;

/// Size in terabytes represented using long double.
using FTerabytes = GTerabytes<long double>;

/// Size in petabytes represented using long double.
using FPetabytes = GPetabytes<long double>;

/// Size in exabytes represented using long double.
using FExabytes = GExabytes<long double>;

/// Size in kibibytes represented using long double.
using FKibibytes = GKibibytes<long double>;

/// Size in mebibytes represented using long double.
using FMebibytes = GMebibytes<long double>;

/// Size in gibibytes represented using long double.
using FGibibytes = GGibibytes<long double>;

/// Size in tebibytes represented using long double.
using FTebibytes = GTebibytes<long double>;

/// Size in pebibytes represented using long double.
using FPebibytes = GPebibytes<long double>;

/// Size in exbibytes represented using long double.
using FExbibytes = GExbibytes<long double>;

template <ArithmeticType T>
constexpr StringLiteral Suffix<GBytes<T>> = "B";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GKilobytes<T>> = "KB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GMegabytes<T>> = "MB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GGigabytes<T>> = "GB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GTerabytes<T>> = "TB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GPetabytes<T>> = "PB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GExabytes<T>> = "EB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GKibibytes<T>> = "KiB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GMebibytes<T>> = "MiB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GGibibytes<T>> = "GiB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GTebibytes<T>> = "TiB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GPebibytes<T>> = "PiB";

template <ArithmeticType T>
constexpr StringLiteral Suffix<GExbibytes<T>> = "EiB";

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

} // namespace downward::utils

#endif
