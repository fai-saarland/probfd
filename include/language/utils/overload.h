//
// Created by Thorsten Klößner on 28.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef LANGUAGE_UTILS_OVERLOAD_H
#define LANGUAGE_UTILS_OVERLOAD_H

namespace language::utils {

template <typename... Bases>
struct overload : Bases... {
    using is_transparent = void;
    using Bases::operator()...;
};

}

#endif //LANGUAGE_UTILS_OVERLOAD_H
