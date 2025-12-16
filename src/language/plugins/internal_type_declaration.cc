#include "language/plugins/internal_type_declaration.h"

using namespace std;

namespace language::plugins {

InternalTypeDeclarationBase::InternalTypeDeclarationBase(
    type_index pointer_type,
    std::string type_identifier,
    std::string synopsis)
    : pointer_type(pointer_type)
    , identifier(std::move(type_identifier))
    , synopsis(std::move(synopsis))
{
}

type_index InternalTypeDeclarationBase::get_pointer_type() const
{
    return pointer_type;
}

string InternalTypeDeclarationBase::get_identifier() const
{
    return identifier;
}

string InternalTypeDeclarationBase::get_class_name() const
{
    return pointer_type.name();
}

string InternalTypeDeclarationBase::get_synopsis() const
{
    return synopsis;
}

} // namespace language::plugins
