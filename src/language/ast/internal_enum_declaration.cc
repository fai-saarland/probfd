#include "language/ast/internal_enum_declaration.h"

#include "language/ast/namespace_level_declaration_visitor.h"

#include "language/utils/demangle.h"

using namespace std;

namespace language::parser {

InternalEnumDeclarationBase::InternalEnumDeclarationBase(
    type_index type,
    std::string type_identifier,
    std::vector<std::string> enumerator_names,
    std::vector<std::string> enumerator_documentation)
    : NamespaceLevelDeclaration(std::move(type_identifier))
    , type(type)
    , enumerator_names(std::move(enumerator_names))
    , enumerator_documentation(std::move(enumerator_documentation))
{
}

void InternalEnumDeclarationBase::document_title(const std::string& title)
{
    this->title = title;
}

void InternalEnumDeclarationBase::document_synopsis(const std::string& synopsis)
{
    this->synopsis = synopsis;
}

type_index InternalEnumDeclarationBase::get_type() const
{
    return type;
}

const std::string& InternalEnumDeclarationBase::get_title() const
{
    return title;
}

const std::string& InternalEnumDeclarationBase::get_synopsis() const
{
    return synopsis;
}

void InternalEnumDeclarationBase::visit(
    NamespaceLevelDeclarationVisitor& visitor)
{
    visitor.accept(*this);
}

} // namespace language::parser
