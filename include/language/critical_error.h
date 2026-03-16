//
// Created by Thorsten Klößner on 20.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_CRITICAL_ERROR_H
#define LANGUAGE_CRITICAL_ERROR_H

#include <format>
#include <stdexcept>

namespace language {

/// Unrecoverable error within the language code.
/// Not meant to be caught except by the main function.
class CriticalError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;

    template <class... Args>
    explicit CriticalError(std::format_string<Args...> fmt, Args&&... args)
        : std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
    {
    }
};
} // namespace language

#endif // LANGUAGE_CRITICAL_ERROR_H
