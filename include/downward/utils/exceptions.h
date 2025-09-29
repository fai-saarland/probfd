#ifndef UTILS_EXCEPTIONS_H
#define UTILS_EXCEPTIONS_H

#include <string>

namespace downward::utils {
// Base class for custom exception types.
class Exception {
protected:
    const std::string msg;

public:
    explicit Exception(const std::string& msg);
    virtual ~Exception() = default;

    std::string get_message() const;
    virtual void print() const;
};

struct TimeoutException {};

struct OutOfMemoryException : std::runtime_error {
    using runtime_error::runtime_error;
};

} // namespace downward::utils

#endif
