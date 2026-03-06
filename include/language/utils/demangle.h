//
// Created by Thorsten Klößner on 20.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_UTILS_DEMANGLE_H
#define LANGUAGE_UTILS_DEMANGLE_H

#include <string>

namespace language::utils {

/// Demangle a type name extracted from a std::type_index.
std::string demangle(const char* name);

} // namespace language::plugins

#endif
