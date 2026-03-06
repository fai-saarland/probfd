#include "language/ast/internal_type_declaration.h"

#include "language/ast/namespace_level_declaration_visitor.h"

using namespace std;

namespace language::parser {

InternalTypeDeclaration::InternalTypeDeclaration(
    type_index index,
    std::string type_identifier,
    std::string synopsis)
    : NamespaceLevelDeclaration(std::move(type_identifier))
    , index(index)
    , synopsis(std::move(synopsis))
{
}

type_index InternalTypeDeclaration::get_pointer_type() const
{
    return index;
}

string InternalTypeDeclaration::get_synopsis() const
{
    return synopsis;
}

void InternalTypeDeclaration::register_declarations(
    Context& context,
    typed_ast::GlobalEnvironment&,
    typed_ast::NamespaceScope& scope,
    typed_ast::TypeRegistry& type_registry) const
{
    scope.create_or_get_type(
        this->get_identifier(),
        type_registry.create_feature_type(*this),
        context);
}

void InternalTypeDeclaration::visit(
    NamespaceLevelDeclarationVisitor& visitor)
{
    visitor.accept(*this);
}

} // namespace language::parser
