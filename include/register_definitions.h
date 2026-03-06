//
// Created by Thorsten Klößner on 31.07.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#ifndef PROBFD_REGISTER_DEFINITIONS_H
#define PROBFD_REGISTER_DEFINITIONS_H

namespace language::parser {
class CompilationContext;
}

namespace probfd {
void register_definitions(language::parser::CompilationContext& ccontext);
}

#endif
