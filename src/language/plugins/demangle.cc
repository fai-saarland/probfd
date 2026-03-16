//
// Created by Thorsten Klößner on 20.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#include "language/plugins/demangle.h"

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace language::plugins {

std::string demangle(const char* name)
{
#ifdef __GNUG__
    int status;

    const std::unique_ptr<char, void (*)(void*)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free};

    return status == 0 ? res.get() : name;
#else
    return name;
#endif
}

} // namespace language::plugins