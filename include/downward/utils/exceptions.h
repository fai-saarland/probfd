#ifndef UTILS_EXCEPTIONS_H
#define UTILS_EXCEPTIONS_H

#include <format>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>

namespace downward::utils {

// Base class for custom exception types.
template <typename Derived>
class Exception : public Derived {
public:
    using Derived::Derived;

    template <class... Args>
    explicit Exception(std::format_string<Args...> fmt, Args&&... args)
        : Derived(std::format(fmt, std::forward<Args>(args)...))
    {
    }

    virtual void print(std::ostream& out) const
    {
        std::print(out, "{}", this->what());
    }
};

class OutOfMemoryException : public Exception<std::runtime_error> {
public:
    using Exception::Exception;
};

class CriticalError : public Exception<std::runtime_error> {
public:
    using Exception::Exception;
};

class InputError : public Exception<std::domain_error> {
public:
    using Exception::Exception;
};

class UnsupportedError : public Exception<std::runtime_error> {
public:
    using Exception::Exception;
};

class UnimplementedError : public Exception<std::runtime_error> {
public:
    using Exception::Exception;
};

class OverflowError : public Exception<std::overflow_error> {
public:
    using Exception::Exception;
};

struct TimeoutException {};

} // namespace downward::utils

#endif
