#include "language/plugins/types.h"

#include "language/plugins/demangle.h"
#include "language/plugins/internal_enum_declaration.h"
#include "language/plugins/internal_function_definition.h"
#include "language/plugins/internal_type_declaration.h"

#include "language/critical_error.h"

#include <typeindex>

using namespace std;

namespace language::plugins {

EmptyListType TypeRegistry::EMPTY_LIST_TYPE;

TypeRegistry::TypeRegistry()
{
    insert_basic_type<bool>();
    insert_basic_type<int>();
    insert_basic_type<double>();
    insert_basic_type<string>();
}

template <typename T>
void TypeRegistry::insert_basic_type()
{
    type_index type = typeid(T);
    registered_types[type] =
        std::make_unique<BasicType>(type, demangle(type.name()));
}

const InternalType&
TypeRegistry::create_feature_type(const InternalTypeDeclarationBase& plugin)
{
    auto [it, inserted] = registered_types.emplace(
        plugin.get_pointer_type(),
        std::make_unique<InternalType>(
            plugin.get_pointer_type(),
            plugin.get_identifier()));

    if (!inserted) {
        throw CriticalError(
            "Creating the FeatureType '{}' but the type '{}' already exists "
            "and has the same type_index.",
            plugin.get_class_name(),
            it->second->name());
    }

    return static_cast<const InternalType&>(*it->second);
}

const EnumType&
TypeRegistry::create_enum_type(const InternalEnumDeclarationBase& plugin)
{
    auto [it, inserted] = registered_types.emplace(
        plugin.get_type(),
        std::make_unique<EnumType>(plugin.get_type(), plugin.get_class_name()));

    if (!inserted) {
        throw CriticalError(
            "Creating the EnumType '{}' but the type '{}' already exists and "
            "has the same type_index.",
            plugin.get_class_name(),
            it->second->name());
    }

    return static_cast<const EnumType&>(*it->second);
}

const ListType& TypeRegistry::create_list_type(const Type& element_type)
{
    const Type* key = &element_type;
    if (!registered_list_types.contains(key)) {
        registered_list_types.insert(
            {key, std::make_unique<ListType>(element_type)});
    }
    return *registered_list_types[key];
}

const FunctionType& TypeRegistry::create_function_type(
    const Type& return_type,
    std::vector<const Type*> arg_types)
{
    return **registered_function_types
                 .insert(
                     std::make_unique<FunctionType>(
                         return_type,
                         std::move(arg_types)))
                 .first;
}

const Type& TypeRegistry::get_nonlist_type(type_index type) const
{
    if (const auto it = registered_types.find(type);
        it != registered_types.end()) {
        return *it->second;
    }

    throw CriticalError("Missing type {}", demangle(type.name()));
}
} // namespace language::plugins
