#pragma once

#include "../value_type.h"

#include <type_traits>
#include <utility>

namespace probabilistic {
namespace algorithms {
namespace value_utils {

template<typename Dual>
struct IncumbentSolution;

template<>
struct IncumbentSolution<std::true_type>
    : public std::pair<value_type::value_t, value_type::value_t> {
    using std::pair<value_type::value_t, value_type::value_t>::pair;
    explicit IncumbentSolution(const value_type::value_t& val)
        : std::pair<value_type::value_t, value_type::value_t>(val, val)
    {
    }
};

template<>
struct IncumbentSolution<std::false_type> {
    explicit IncumbentSolution()
        : first(value_type::zero)
    {
    }
    explicit IncumbentSolution(const value_type::value_t& val)
        : first(val)
    {
    }
    value_type::value_t first;
};

template<typename Dual>
inline int update_incumbent(
    IncumbentSolution<Dual>& new_value,
    const IncumbentSolution<Dual>& tval);

template<>
inline int
update_incumbent<std::true_type>(
    IncumbentSolution<std::true_type>& new_value,
    const IncumbentSolution<std::true_type>& tval)
{
    const int r =
        value_type::approx_greater(value_type::eps)(tval.first, new_value.first)
        ? 1
        : (value_type::approx_equal(value_type::eps)(
               tval.first, new_value.first)
               ? 0
               : -1);
    if (tval.first > new_value.first) {
        new_value.first = tval.first;
    }
    if (tval.second > new_value.second) {
        new_value.second = tval.second;
    }
    return r;
}

template<>
inline int
update_incumbent<std::false_type>(
    IncumbentSolution<std::false_type>& new_value,
    const IncumbentSolution<std::false_type>& tval)
{
    const int r =
        value_type::approx_greater(value_type::eps)(tval.first, new_value.first)
        ? 1
        : (value_type::approx_equal(value_type::eps)(
               tval.first, new_value.first)
               ? 0
               : -1);
    if (tval.first > new_value.first) {
        new_value.first = tval.first;
    }
    return r;
}

template<typename StateInfo>
inline void
add(IncumbentSolution<std::true_type>& res,
    const value_type::value_t& prob,
    const StateInfo& succ_info)
{
    res.first += prob * succ_info.value;
    res.second += prob * succ_info.value2;
}

template<typename StateInfo>
inline void
add(IncumbentSolution<std::false_type>& res,
    const value_type::value_t& prob,
    const StateInfo& succ_info)
{
    res.first += prob * succ_info.value;
}

template<>
inline void
add(IncumbentSolution<std::false_type>& res,
    const value_type::value_t& prob,
    const value_type::value_t& val)
{
    res.first += prob * val;
}

template<typename Dual>
inline void mult(IncumbentSolution<Dual>& res, const value_type::value_t& prob);

template<>
inline void
mult<std::true_type>(
    IncumbentSolution<std::true_type>& res,
    const value_type::value_t& prob)
{
    res.first *= prob;
    res.second *= prob;
}

template<>
inline void
mult<std::false_type>(
    IncumbentSolution<std::false_type>& res,
    const value_type::value_t& prob)
{
    res.first *= prob;
}

} // namespace value_utils
} // namespace algorithms
} // namespace probabilistic
