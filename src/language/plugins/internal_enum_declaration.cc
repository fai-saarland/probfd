#include "language/plugins/internal_enum_declaration.h"

#include "language/plugins/demangle.h"

using namespace std;

namespace language::plugins {

InternalEnumDeclarationBase::InternalEnumDeclarationBase(
    type_index type,
    std::vector<std::string> enumerator_documentation)
    : type(type)
    , enumerator_documentation(std::move(enumerator_documentation))
{
}

type_index InternalEnumDeclarationBase::get_type() const
{
    return type;
}

string InternalEnumDeclarationBase::get_class_name() const
{
    return demangle(type.name());
}

} // namespace language::plugins
