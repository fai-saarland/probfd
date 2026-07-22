#include "language/plugins/type_registry.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include <chrono>
#include <ranges>
#include <typeindex>

using namespace std;

namespace language::plugins {

SymbolType TypeRegistry::SYMBOL_TYPE;
EmptyListType TypeRegistry::EMPTY_LIST_TYPE;
BasicType TypeRegistry::NO_TYPE = BasicType(typeid(void), "<no type>");

TypeRegistry::TypeRegistry()
{
    insert_basic_type<bool>();
    insert_basic_type<string>();
    insert_basic_type<int>();
    insert_basic_type<double>();
}

template <typename T>
void TypeRegistry::insert_basic_type()
{
    type_index type = typeid(T);

    std::string name;

    if constexpr (std::same_as<T, bool>) {
        name = "bool";
    } else if constexpr (std::same_as<T, float>) {
        name = "float";
    } else if constexpr (std::same_as<T, double>) {
        name = "double";
    } else if constexpr (std::same_as<T, int>) {
        name = "int";
    } else if constexpr (std::same_as<T, std::string>) {
        name = "string";
    } else {
        name = type.name();
    }

    registered_types[type] = std::make_unique<BasicType>(type, name);
}

const FeatureType&
TypeRegistry::create_feature_type(const CategoryPlugin& plugin)
{
    const type_index type = plugin.get_pointer_type();
    if (registered_types.contains(type)) {
        throw RegistryError<std::runtime_error>(
            "Creating the FeatureType '{}' but the type '{}' already exists "
            "and has the same type_index.",
            plugin.get_class_name(),
            registered_types[type]->name());
    }

    auto type_ptr = std::make_unique<FeatureType>(
        plugin.get_pointer_type(),
        plugin.get_category_name(),
        plugin.get_synopsis());

    const FeatureType& type_ref = *type_ptr;
    registered_types[type] = move(type_ptr);
    return type_ref;
}

const EnumType& TypeRegistry::create_enum_type(const EnumPlugin& plugin)
{
    type_index type = plugin.get_type();
    const EnumInfo& values = plugin.get_enum_info();
    if (registered_types.contains(type)) {
        throw RegistryError<std::runtime_error>(
            "Creating the EnumType '{}' but the type '{}' already exists and "
            "has the same type_index.",
            plugin.get_class_name(),
            registered_types[type]->name());
    }

    auto type_ptr = std::make_unique<EnumType>(type, values);
    const EnumType& type_ref = *type_ptr;
    registered_types[type] = move(type_ptr);
    return type_ref;
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

const Type& TypeRegistry::get_nonlist_type(type_index type) const
{
    if (!registered_types.contains(type)) { return NO_TYPE; }
    return *registered_types.at(type);
}

} // namespace language::plugins
