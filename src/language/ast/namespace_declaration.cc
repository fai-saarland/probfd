//
// Created by Thorsten Klößner on 28.12.2025.
// Copyright (c) 2025 ProbFD contributors.
//

#include "language/ast/namespace_declaration.h"

#include "language/ast/namespace_level_declaration_visitor.h"

#include "language/typed_ast/variable_environment.h"

namespace language::parser {

void NamespaceDeclaration::register_declarations(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::NamespaceScope& scope,
    typed_ast::TypeRegistry& type_registry) const
{
    typed_ast::NamespaceScope& nested_scope =
        scope.create_or_get_namespace(get_identifier(), context);

    for (const auto& decl : global_declarations) {
        decl->register_declarations(context, env, nested_scope, type_registry);
    }
}

void NamespaceDeclaration::visit(NamespaceLevelDeclarationVisitor& visitor)
{
    visitor.accept(*this);
}

} // namespace language::parser