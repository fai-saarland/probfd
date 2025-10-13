//
// Created by Thorsten Klößner on 10.10.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef DOWNWARD_UTILS_UNITS_H
#define DOWNWARD_UTILS_UNITS_H

#include "downward/concepts.h"

#include "downward/utils/string_literal.h"

#include <chrono>
#include <format>
#include <numeric>
#include <ratio>

namespace downward::utils {

/// Custom ratio types
using kibi = std::ratio<2LL << 10>;
using mebi = std::ratio<2LL << 20>;
using gibi = std::ratio<2LL << 30>;
using tebi = std::ratio<2LL << 40>;
using pebi = std::ratio<2LL << 50>;
using exbi = std::ratio<2LL << 60>;

/// Models specializations of std::ratio
template <typename T>
concept RatioType = SpecializationCTP<T, std::ratio>;

namespace detail {
template <typename T, template <ArithmeticType, RatioType> typename U>
struct SpecializationHelperUnit : std::false_type {};

template <
    ArithmeticType A,
    RatioType B,
    template <ArithmeticType, RatioType> typename U>
struct SpecializationHelperUnit<U<A, B>, U> : std::true_type {};

template <typename T, template <ArithmeticType, RatioType> typename U>
struct DerivedFromSpecializationHelperUnit : std::false_type {};

template <
    ArithmeticType A,
    RatioType B,
    template <ArithmeticType, RatioType> typename V,
    template <ArithmeticType, RatioType> typename U>
struct DerivedFromSpecializationHelperUnit<U<A, B>, V>
    : std::bool_constant<std::derived_from<U<A, B>, V<A, B>>> {};
} // namespace detail

template <ArithmeticType Rep, RatioType Period>
class unit;

template <typename T>
concept DerivedFromSpecializationUnit =
    detail::DerivedFromSpecializationHelperUnit<T, unit>::value;

/// Specializations specify the unit suffix for the unit type.
template <DerivedFromSpecializationUnit T>
struct SuffixStruct {
    static_assert(false, "Undefined symbol");
};

/// StringLiteral suffix for the unit type.
template <DerivedFromSpecializationUnit T>
inline constexpr auto Suffix = SuffixStruct<T>::value;

template <ArithmeticType Rep, RatioType Period>
class unit {
protected:
    Rep value;

public:
    using period = Period;
    using rep = Rep;

    explicit constexpr unit(Rep value)
        : value(value)
    {
    }

    constexpr auto count() const { return value; }

    template <std::derived_from<unit> T>
    friend std::ostream& operator<<(std::ostream& out, const T& u)
    {
        return out << u.value << Suffix<T>.value.data();
    }
};

// Arithmetic operators for unit types

template <
    class Rep1,
    class Period1,
    class Rep2,
    class Period2,
    template <ArithmeticType, RatioType> typename U>
    requires std::derived_from<U<Rep1, Period1>, unit<Rep1, Period1>> &&
             std::derived_from<U<Rep2, Period2>, unit<Rep2, Period2>>
std::common_type_t<U<Rep1, Period1>, U<Rep2, Period2>> constexpr
operator+(const U<Rep1, Period1>& lhs, const U<Rep2, Period2>& rhs)
{
    using CD = std::common_type_t<U<Rep1, Period1>, U<Rep2, Period2>>;
    return CD(CD(lhs).count() + CD(rhs).count());
}

template <
    class Rep1,
    class Period1,
    class Rep2,
    class Period2,
    template <ArithmeticType, RatioType> typename U>
    requires std::derived_from<U<Rep1, Period1>, unit<Rep1, Period1>> &&
             std::derived_from<U<Rep2, Period2>, unit<Rep2, Period2>>
std::common_type_t<U<Rep1, Period1>, U<Rep2, Period2>> constexpr
operator-(const U<Rep1, Period1>& lhs, const U<Rep2, Period2>& rhs)
{
    using CD = std::common_type_t<U<Rep1, Period1>, U<Rep2, Period2>>;
    return CD(CD(lhs).count() - CD(rhs).count());
}

template <
    class Rep1,
    class Rep2,
    class Period,
    template <ArithmeticType, RatioType> typename U>
    requires std::derived_from<U<Rep1, Period>, unit<Rep1, Period>> &&
             std::derived_from<U<Rep2, Period>, unit<Rep2, Period>>
std::common_type_t<std::common_type_t<Rep1, Rep2>, Period> constexpr
operator*(const U<Rep1, Period>& lhs, const Rep2& rhs)
{
    using CD = std::common_type_t<std::common_type_t<Rep1, Rep2>, Period>;
    return CD(CD(lhs).count() * rhs);
}

template <
    class Rep1,
    class Rep2,
    class Period,
    template <ArithmeticType, RatioType> typename U>
    requires std::derived_from<U<Rep1, Period>, unit<Rep1, Period>> &&
             std::derived_from<U<Rep2, Period>, unit<Rep2, Period>>
std::common_type_t<std::common_type_t<Rep1, Rep2>, Period> constexpr
operator*(const Rep1& lhs, const U<Rep2, Period>& rhs)
{
    using CD = std::common_type_t<std::common_type_t<Rep1, Rep2>, Period>;
    return CD(lhs * CD(rhs).count());
}

// Cast between related unit types
template <
    typename To,
    ArithmeticType Rep,
    RatioType Period,
    template <ArithmeticType, RatioType> typename U>
    requires detail::SpecializationHelperUnit<To, U>::value
constexpr To unit_cast(const U<Rep, Period>& dur) noexcept
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

} // namespace downward::utils

namespace detail {
template <downward::ArithmeticType Rep, downward::utils::RatioType Period>
void DerivedFromUnitImpl(const downward::utils::unit<Rep, Period>&);
}

/// This concept is satisfied if T has exactly one public base class which is a
/// specialization of unit.
template <class T>
concept DerivedFromUnit =
    requires(const T& t) { detail::DerivedFromUnitImpl(t); };

// std::common_type specialization for unit-derived types.
// Instantiating this template with an incomplete type is undefined behaviour,
// but so is instantiating the base template with incomplete types, so don't
// do it.
template <
    downward::ArithmeticType Rep1,
    downward::utils::RatioType Period1,
    downward::ArithmeticType Rep2,
    downward::utils::RatioType Period2,
    template <downward::ArithmeticType, downward::utils::RatioType> typename U>
    requires DerivedFromUnit<U<Rep1, Period1>> &&
             DerivedFromUnit<U<Rep2, Period2>>
struct std::common_type<U<Rep1, Period1>, U<Rep2, Period2>> {
    using type =
        U<std::common_type_t<Rep1, Rep2>,
          std::ratio<
              std::gcd(Period1::num, Period2::num),
              std::lcm(Period1::den, Period2::den)>>;
};

// Formatting support for unit types
template <
    downward::ArithmeticType Rep,
    downward::utils::RatioType Period,
    template <downward::ArithmeticType, downward::utils::RatioType> typename U>
    requires std::
        derived_from<U<Rep, Period>, downward::utils::unit<Rep, Period>>
    struct std::formatter<U<Rep, Period>> {
    using Base = U<Rep, Period>;
    std::formatter<Rep> underlying_;

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class FmtContext>
    typename FmtContext::iterator
    format(const U<Rep, Period>& size, FmtContext& ctx) const
    {
        using downward::utils::string_literals::operator""_sl;

        // "{}<suffix>"
        // Needs to have a static address for std::format_string
        static constexpr auto t =
            downward::utils::concat("{}"_sl, downward::utils::Suffix<Base>);

        return std::format_to(ctx.out(), t, size.count());
    }
};

#endif
