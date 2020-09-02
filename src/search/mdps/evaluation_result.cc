#include "evaluation_result.h"

namespace probabilistic {

EvaluationResult::EvaluationResult(bool x, value_type::value_t val)
    : bool_(x)
    , value_(val)
{
}

EvaluationResult::operator bool() const { return bool_; }

EvaluationResult::operator value_type::value_t() const { return value_; }

value_type::value_t
EvaluationResult::operator+(const value_type::value_t& val) const
{
    return value_ + val;
}

value_type::value_t
EvaluationResult::operator-(const value_type::value_t& val) const
{
    return value_ - val;
}

value_type::value_t
EvaluationResult::operator*(const value_type::value_t& val) const
{
    return value_ * val;
}

value_type::value_t
EvaluationResult::operator/(const value_type::value_t& val) const
{
    return value_ / val;
}

value_type::value_t
operator+(const value_type::value_t& x, const EvaluationResult& y)
{
    return y + x;
}

value_type::value_t
operator-(const value_type::value_t& x, const EvaluationResult& y)
{
    return x - (value_type::value_t)y;
}

value_type::value_t
operator*(const value_type::value_t& x, const EvaluationResult& y)
{
    return y * x;
}

value_type::value_t
operator/(const value_type::value_t& x, const EvaluationResult& y)
{
    return x / (value_type::value_t)y;
}

} // namespace probabilistic
