#ifndef LANGUAGE_AST_NAMESPACE_DECLARATION_H
#define LANGUAGE_AST_NAMESPACE_DECLARATION_H

#include "language/ast/namespace_level_declaration.h"

namespace language::parser {

class NamespaceDeclaration
    : public NamespaceLevelDeclaration
    , public NamespaceLevelDeclarationList {
public:
    using NamespaceLevelDeclaration::NamespaceLevelDeclaration;

    void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::NamespaceScope& scope,
        typed_ast::TypeRegistry& type_registry) const override;

    void visit(NamespaceLevelDeclarationVisitor& visitor) override;
};

NamespaceDeclaration& insert_namespace_declaration(
    NamespaceLevelDeclarationList& list,
    const std::string& name);

} // namespace language::parser

#endif
