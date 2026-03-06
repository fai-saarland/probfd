//
// Created by Thorsten Klößner on 02.01.2026.
// Copyright (c) 2026 ProbFD contributors.
//

#ifndef LANGUAGE_TYPED_AST_TYPE_ALIASES_H
#define LANGUAGE_TYPED_AST_TYPE_ALIASES_H

#include <any>

namespace language::typed_ast {
class ConstructContext;
}

namespace language::typed_ast {

using AnyFunctionType = std::any (*)(ConstructContext& stack);
}

#endif // LANGUAGE_TYPED_AST_TYPE_ALIASES_H
