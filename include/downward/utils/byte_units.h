//
// Created by Thorsten Klößner on 10.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_UTILS_BYTE_UNITS_H
#define DOWNWARD_UTILS_BYTE_UNITS_H

#include "downward/utils/units.h"

namespace downward::utils {

template <ArithmeticType Rep, RatioType Period>
class ByteSize : public unit<Rep, Period> {
    // Inherit constructor
    using ByteSize::unit::unit;
};

template <typename T>
using GBytes = ByteSize<T, std::ratio<1>>;
template <typename T>
using GKilobytes = ByteSize<T, std::kilo>;
template <typename T>
using GMegabytes = ByteSize<T, std::mega>;
template <typename T>
using GGigabytes = ByteSize<T, std::giga>;
template <typename T>
using GTerabytes = ByteSize<T, std::tera>;
template <typename T>
using GPetabytes = ByteSize<T, std::peta>;
template <typename T>
using GExabytes = ByteSize<T, std::exa>;

template <typename T>
using GKibibytes = ByteSize<T, kibi>;
template <typename T>
using GMebibytes = ByteSize<T, mebi>;
template <typename T>
using GGibibytes = ByteSize<T, gibi>;
template <typename T>
using GTebibytes = ByteSize<T, tebi>;
template <typename T>
using GPebibytes = ByteSize<T, pebi>;
template <typename T>
using GExbibytes = ByteSize<T, exbi>;

using Bytes = GBytes<std::intmax_t>;
using Kilobytes = GKilobytes<std::intmax_t>;
using Megabytes = GMegabytes<std::intmax_t>;
using Gigabytes = GGigabytes<std::intmax_t>;
using Terabytes = GTerabytes<std::intmax_t>;
using Petabytes = GPetabytes<std::intmax_t>;
using Exabytes = GExabytes<std::intmax_t>;

using Kibibytes = GKibibytes<std::intmax_t>;
using Mebibytes = GMebibytes<std::intmax_t>;
using Gibibytes = GGibibytes<std::intmax_t>;
using Tebibytes = GTebibytes<std::intmax_t>;
using Pebibytes = GPebibytes<std::intmax_t>;
using Exbibytes = GExbibytes<std::intmax_t>;

using FBytes = GBytes<long double>;
using FKilobytes = GKilobytes<long double>;
using FMegabytes = GMegabytes<long double>;
using FGigabytes = GGigabytes<long double>;
using FTerabytes = GTerabytes<long double>;
using FPetabytes = GPetabytes<long double>;
using FExabytes = GExabytes<long double>;

using FKibibytes = GKibibytes<long double>;
using FMebibytes = GMebibytes<long double>;
using FGibibytes = GGibibytes<long double>;
using FTebibytes = GTebibytes<long double>;
using FPebibytes = GPebibytes<long double>;
using FExbibytes = GExbibytes<long double>;

template <ArithmeticType T>
struct SuffixStruct<GBytes<T>> {
    static constexpr StringLiteral value = "B";
};

template <ArithmeticType T>
struct SuffixStruct<GKilobytes<T>> {
    static constexpr StringLiteral value = "KB";
};

template <ArithmeticType T>
struct SuffixStruct<GMegabytes<T>> {
    static constexpr StringLiteral value = "MB";
};

template <ArithmeticType T>
struct SuffixStruct<GGigabytes<T>> {
    static constexpr StringLiteral value = "GB";
};

template <ArithmeticType T>
struct SuffixStruct<GTerabytes<T>> {
    static constexpr StringLiteral value = "TB";
};

template <ArithmeticType T>
struct SuffixStruct<GPetabytes<T>> {
    static constexpr StringLiteral value = "PB";
};

template <ArithmeticType T>
struct SuffixStruct<GExabytes<T>> {
    static constexpr StringLiteral value = "EB";
};

template <ArithmeticType T>
struct SuffixStruct<GKibibytes<T>> {
    static constexpr StringLiteral value = "KiB";
};

template <ArithmeticType T>
struct SuffixStruct<GMebibytes<T>> {
    static constexpr StringLiteral value = "MiB";
};

template <ArithmeticType T>
struct SuffixStruct<GGibibytes<T>> {
    static constexpr StringLiteral value = "GiB";
};

template <ArithmeticType T>
struct SuffixStruct<GTebibytes<T>> {
    static constexpr StringLiteral value = "TiB";
};

template <ArithmeticType T>
struct SuffixStruct<GPebibytes<T>> {
    static constexpr StringLiteral value = "PiB";
};

template <ArithmeticType T>
struct SuffixStruct<GExbibytes<T>> {
    static constexpr StringLiteral value = "EiB";
};

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
