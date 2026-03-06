//
// Created by Thorsten Klößner on 28.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_UTILS_STRING_MAP_H
#define LANGUAGE_UTILS_STRING_MAP_H

#include "language/utils/overload.h"

#include <map>
#include <string>
#include <string_view>
#include <unordered_map>

namespace language::utils {

struct char_pointer_hash {
    auto operator()(const char* ptr) const noexcept
    {
        return std::hash<std::string_view>{}(ptr);
    }
};

using transparent_string_hash = overload<
    std::hash<std::string>,
    std::hash<std::string_view>,
    char_pointer_hash>;

template <typename T>
using UnorderedStringMap =
    std::unordered_map<std::string_view, T, transparent_string_hash>;

template <typename T>
using StringMap = std::map<std::string_view, T, std::less<>>;

} // namespace language::utils

#endif // LANGUAGE_UTILS_STRING_MAP_H
