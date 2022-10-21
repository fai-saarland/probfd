#ifndef MDPS_EVALUATION_RESULT_H
#define MDPS_EVALUATION_RESULT_H

#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Holds a state value and a flag with varying purposes.
 */
class EvaluationResult {
public:
    EvaluationResult() = default;
    EvaluationResult(bool x, value_type::value_t val);

    value_type::value_t operator+(const value_type::value_t& val) const;
    value_type::value_t operator-(const value_type::value_t& val) const;
    value_type::value_t operator*(const value_type::value_t& val) const;
    value_type::value_t operator/(const value_type::value_t& val) const;

    /// Converts to a bool indicating whether the state is terminal.
    explicit operator bool() const;

    /// Converts to a value_t representing the state value.
    explicit operator value_type::value_t() const;

private:
    bool bool_;                 // state flag
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

} // namespace probfd

#endif // __EVALUATION_RESULT_H__