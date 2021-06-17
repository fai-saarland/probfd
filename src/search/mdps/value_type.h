#pragma once

/// Namespace for classes related to search in probabilistic state space models.
namespace probabilistic {

/// Namespace dedicated to state value types.
namespace value_type {

/// Typedef for the state value type
using value_t = double;

extern const value_t one;
extern const value_t zero;
extern const value_t inf;
extern const value_t eps;

/// The global epsilon precision value.
extern value_t g_epsilon;


extern value_t from_double(double d);
extern value_t from_fraction(int nom, int denom);
extern value_t cap(const value_t& val, const value_t& maxval);
extern value_t abs(const value_t& val);

struct approx_equal {
    approx_equal(const value_t& val = g_epsilon);
    bool operator()(const value_t& v1, const value_t& v2) const;
    const value_t eps_;
};

struct approx_less {
    approx_less(const value_t& val = g_epsilon);
    bool operator()(const value_t& v1, const value_t& v2) const;
    const value_t eps_;
};

struct approx_greater {
    approx_greater(const value_t& val = g_epsilon);
    bool operator()(const value_t& v1, const value_t& v2) const;
    const value_t eps_;
};

} // namespace value_type
} // namespace probabilistic
