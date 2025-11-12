//
// Created by Thorsten Klößner on 31.07.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_REGISTER_DEFINITIONS_H
#define PROBFD_REGISTER_DEFINITIONS_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd {
void register_definitions(downward::cli::plugins::Registry& raw_registry);
}

#endif
