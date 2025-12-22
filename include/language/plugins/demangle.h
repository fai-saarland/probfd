//
// Created by Thorsten Klößner on 20.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_PLUGINS_DEMANGLE_H
#define LANGUAGE_PLUGINS_DEMANGLE_H

#include <string>

namespace language::plugins {

/// Demangle a type name extracted from a std::type_index.
std::string demangle(const char* name);

}

#endif
