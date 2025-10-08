#ifndef UTILS_EXCEPTIONS_H
#define UTILS_EXCEPTIONS_H

#include <iosfwd>
#include <stdexcept>
#include <string>

namespace downward::utils {

// Base class for custom exception types.
class Exception : public std::runtime_error {
public:
    using runtime_error::runtime_error;

    virtual void print(std::ostream& out) const;
};

struct TimeoutException {};

struct OutOfMemoryException : Exception {
    using Exception::Exception;
};

} // namespace downward::utils

#endif
