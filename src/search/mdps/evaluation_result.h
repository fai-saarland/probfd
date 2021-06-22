#pragma once

#include "value_type.h"

namespace probabilistic {

class EvaluationResult {
public:
    EvaluationResult(bool x, value_type::value_t val);

    value_type::value_t operator+(const value_type::value_t& val) const;
    value_type::value_t operator-(const value_type::value_t& val) const;
    value_type::value_t operator*(const value_type::value_t& val) const;
    value_type::value_t operator/(const value_type::value_t& val) const;

    explicit operator bool() const;
    explicit operator value_type::value_t() const;

private:
    bool bool_; // terminal state flag (true -> considered terminal)
    value_type::value_t value_; // value estimate
};

value_type::value_t
operator+(const value_type::value_t& x, const EvaluationResult& y);

value_type::value_t
operator-(const value_type::value_t& x, const EvaluationResult& y);

value_type::value_t
operator*(const value_type::value_t& x, const EvaluationResult& y);

value_type::value_t
operator/(const value_type::value_t& x, const EvaluationResult& y);

} // namespace probabilistic
