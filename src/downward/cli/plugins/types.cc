#include "downward/cli/plugins/types.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/strings.h"

#include <chrono>
#include <sstream>
#include <typeindex>

using namespace std;

namespace downward::cli::plugins {

bool Type::is_basic_type() const
{
    return false;
}

bool Type::is_feature_type() const
{
    return false;
}

bool Type::is_list_type() const
{
    return false;
}

bool Type::is_empty_list_type() const
{
    return false;
}

bool Type::is_enum_type() const
{
    return false;
}

bool Type::is_symbol_type() const
{
    return false;
}

bool Type::is_function_type() const
{
    return false;
}

bool Type::can_convert_into(const Type& other) const
{
    return *this == other;
}

FunctionType::FunctionType(
    const Type& return_type,
    std::vector<ArgumentInfo> arguments)
    : return_type(return_type)
    , arguments(std::move(arguments))
{
}

bool FunctionType::operator==(const Type& other) const
{
    return this == &other;
}

std::string FunctionType::name() const
{
    return std::format(
        "{}{}",
        return_type.name(),
        arguments | std::views::transform([](const ArgumentInfo& info) {
            return info.type.name();
        }));
}

size_t FunctionType::get_hash() const
{
    // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
    std::size_t seed = arguments.size() + 1;

    seed ^= return_type.get_hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    for (const auto& info : arguments) {
        seed ^= info.type.get_hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    return seed;
}

BasicType::BasicType(type_index type, const string& class_name)
    : type(type)
    , class_name(class_name)
{
}

bool BasicType::operator==(const Type& other) const
{
    const BasicType* other_ptr = dynamic_cast<const BasicType*>(&other);
    return other_ptr && type == other_ptr->type;
}

bool BasicType::is_basic_type() const
{
    return true;
}

const type_index& BasicType::get_basic_type_index() const
{
    return type;
}

bool BasicType::can_convert_into(const Type& other) const
{
    return Type::can_convert_into(other) ||
           (other.is_basic_type() && get_basic_type_index() == typeid(int) &&
            static_cast<const BasicType&>(other).get_basic_type_index() ==
                typeid(double));
}

string BasicType::name() const
{
    return class_name;
}

size_t BasicType::get_hash() const
{
    return hash<type_index>()(type);
}

FeatureType::FeatureType(
    type_index pointer_type,
    const string& type_name)
    : pointer_type(pointer_type)
    , type_name(type_name)
{
}

bool FeatureType::operator==(const Type& other) const
{
    const FeatureType* other_ptr = dynamic_cast<const FeatureType*>(&other);
    return other_ptr && pointer_type == other_ptr->pointer_type;
}

bool FeatureType::is_feature_type() const
{
    return true;
}

string FeatureType::name() const
{
    return type_name;
}

size_t FeatureType::get_hash() const
{
    return hash<type_index>()(typeid(FeatureType)) ^
           hash<type_index>()(pointer_type);
}

std::type_index FeatureType::get_type_index() const
{
    return pointer_type;
}

ListType::ListType(const Type& nested_type)
    : nested_type(nested_type)
{
}

bool ListType::operator==(const Type& other) const
{
    const ListType* other_ptr = dynamic_cast<const ListType*>(&other);
    return other_ptr && nested_type == other_ptr->nested_type;
}

bool ListType::is_list_type() const
{
    return true;
}

const Type& ListType::get_nested_type() const
{
    return nested_type;
}

bool ListType::can_convert_into(const Type& other) const
{
    return other.is_list_type() &&
           nested_type.can_convert_into(
               static_cast<const ListType&>(other).get_nested_type());
}

string ListType::name() const
{
    return "list of " + nested_type.name();
}

size_t ListType::get_hash() const
{
    return hash<type_index>()(typeid(ListType)) ^ nested_type.get_hash();
}

bool EmptyListType::operator==(const Type& other) const
{
    const EmptyListType* other_ptr = dynamic_cast<const EmptyListType*>(&other);
    return other_ptr;
}

bool EmptyListType::is_list_type() const
{
    return true;
}

bool EmptyListType::is_empty_list_type() const
{
    return true;
}

bool EmptyListType::can_convert_into(const Type& other) const
{
    return other.is_list_type();
}

string EmptyListType::name() const
{
    return "empty list";
}

size_t EmptyListType::get_hash() const
{
    return hash<type_index>()(typeid(EmptyListType));
}

EnumType::EnumType(type_index type, const EnumInfo& documented_values)
    : type(type)
    , documented_values(documented_values)
{
    values.reserve(documented_values.size());
    for (const auto& value : documented_values | views::keys) {
        values.push_back(utils::tolower(value));
    }
}

bool EnumType::operator==(const Type& other) const
{
    const EnumType* other_ptr = dynamic_cast<const EnumType*>(&other);
    return other_ptr && type == other_ptr->type;
}

bool EnumType::is_enum_type() const
{
    return true;
}

int EnumType::get_enum_index(const string& value, utils::Context& context) const
{
    const auto it = ranges::find(values, value);
    const int enum_index = static_cast<int>(it - values.begin());
    if (enum_index >= static_cast<int>(values.size())) {
        context.error("Invalid enum value: {}\nOptions: {:n}", value, values);
    }
    return enum_index;
}

const EnumInfo& EnumType::get_documented_enum_values() const
{
    return documented_values;
}

string EnumType::name() const
{
    return std::format("{{{:n}}}", values);
}

size_t EnumType::get_hash() const
{
    size_t hash_value = 0;
    for (const string& value : values) { hash_value ^= hash<string>()(value); }
    return hash_value;
}

bool SymbolType::operator==(const Type& other) const
{
    return other.is_symbol_type();
}

bool SymbolType::is_symbol_type() const
{
    return true;
}

bool SymbolType::can_convert_into(const Type& other) const
{
    return (*this == other) || other.is_enum_type();
}

string SymbolType::name() const
{
    return "symbol";
}

size_t SymbolType::get_hash() const
{
    return hash<type_index>()(typeid(SymbolType));
}

std::any convert(
    const std::any& value,
    const Type& from_type,
    const Type& to_type,
    utils::Context& context)
{
    if (from_type == to_type) {
        return value;
    } else if (
        from_type.is_basic_type() &&
        static_cast<const BasicType&>(from_type).get_basic_type_index() ==
            typeid(int) &&
        to_type.is_basic_type() &&
        static_cast<const BasicType&>(to_type).get_basic_type_index() ==
            typeid(double)) {
        int int_value = any_cast<int>(value);
        if (int_value == numeric_limits<int>::max()) {
            return std::any(numeric_limits<double>::infinity());
        } else if (int_value == numeric_limits<int>::min()) {
            return std::any(-numeric_limits<double>::infinity());
        }
        return std::any(static_cast<double>(int_value));
    } else if (from_type.is_symbol_type() && to_type.is_enum_type()) {
        string str_value = any_cast<string>(value);
        return std::any(
            static_cast<const EnumType&>(to_type).get_enum_index(
                str_value,
                context));
    } else if (from_type.is_list_type() && to_type.is_list_type()) {
        if (from_type.is_empty_list_type()) { return value; }

        const Type& from_nested_type =
            static_cast<const ListType&>(from_type).get_nested_type();
        const Type& to_nested_type =
            static_cast<const ListType&>(to_type).get_nested_type();

        if (from_nested_type.can_convert_into(to_nested_type)) {
            const vector<std::any>& elements =
                any_cast<vector<std::any>>(value);
            vector<std::any> converted_elements;
            converted_elements.reserve(elements.size());
            for (const std::any& element : elements) {
                converted_elements.push_back(convert(
                    element,
                    from_nested_type,
                    to_nested_type,
                    context));
            }
            return std::any(converted_elements);
        }
    }

    throw utils::CriticalError(
        "Cannot convert {} to {}.",
        from_type.name(),
        to_type.name());
}

SymbolType TypeRegistry::SYMBOL_TYPE;
EmptyListType TypeRegistry::EMPTY_LIST_TYPE;

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
    auto [it, inserted] = registered_types.emplace(
        plugin.get_pointer_type(),
        std::make_unique<FeatureType>(
            plugin.get_pointer_type(),
            plugin.get_category_name()));

    if (!inserted) {
        throw utils::CriticalError(
            "Creating the FeatureType '{}' but the type '{}' already exists "
            "and has the same type_index.",
            plugin.get_class_name(),
            it->second->name());
    }

    return static_cast<const FeatureType&>(*it->second);
}

const EnumType& TypeRegistry::create_enum_type(const EnumPlugin& plugin)
{
    auto [it, inserted] = registered_types.emplace(
        plugin.get_type(),
        std::make_unique<EnumType>(plugin.get_type(), plugin.get_enum_info()));

    if (!inserted) {
        throw utils::CriticalError(
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
    if (!registered_list_types.count(key)) {
        registered_list_types.insert(
            {key, std::make_unique<ListType>(element_type)});
    }
    return *registered_list_types[key];
}

const FunctionType& TypeRegistry::create_function_type(
    const Type& return_type,
    std::vector<ArgumentInfo> arg_types)
{
    return **registered_function_types
                 .insert(std::make_unique<FunctionType>(return_type, arg_types))
                 .first;
}

const Type& TypeRegistry::get_nonlist_type(type_index type) const
{
    if (const auto it = registered_types.find(type);
        it != registered_types.end()) {
        return *it->second;
    }

    throw utils::CriticalError("Missing type {}", type.name());
}
} // namespace downward::cli::plugins
