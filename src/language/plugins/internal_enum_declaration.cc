#include "language/plugins/internal_enum_declaration.h"

using namespace std;

namespace language::plugins {

InternalEnumDeclarationBase::InternalEnumDeclarationBase(
    type_index type,
    initializer_list<pair<string, string>> enum_values)
    : type(type)
    , enum_info(enum_values)
{
}

type_index InternalEnumDeclarationBase::get_type() const
{
    return type;
}

string InternalEnumDeclarationBase::get_class_name() const
{
    return type.name();
}

const EnumInfo& InternalEnumDeclarationBase::get_enum_info() const
{
    return enum_info;
}

} // namespace language::plugins
