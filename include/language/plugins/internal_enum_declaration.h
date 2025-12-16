#ifndef LANGUAGE_PLUGINS_INTERNAL_ENUM_DECLARATION_H
#define LANGUAGE_PLUGINS_INTERNAL_ENUM_DECLARATION_H

#include "language/plugins/registry_types.h"

#include <string>
#include <type_traits>
#include <typeindex>

namespace language::plugins {

class InternalEnumDeclarationBase {
    std::type_index type;
    EnumInfo enum_info;

protected:
    InternalEnumDeclarationBase(
        std::type_index type,
        std::initializer_list<std::pair<std::string, std::string>> enum_values);

public:
    std::type_index get_type() const;
    std::string get_class_name() const;
    const EnumInfo& get_enum_info() const;
};

template <typename T>
    requires std::is_enum_v<T>
class InternalEnumDeclaration : public InternalEnumDeclarationBase {
public:
    InternalEnumDeclaration(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
        : InternalEnumDeclarationBase(typeid(T), enum_values)
    {
    }
};

} // namespace language::plugins

#endif
