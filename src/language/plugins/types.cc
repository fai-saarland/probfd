#include "language/plugins/types.h"

#include "language/plugins/plugin.h"

#include "downward/utils/strings.h"
#include "downward/utils/system.h"

#include <chrono>
#include <sstream>
#include <typeindex>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

using namespace std;

namespace {

std::string demangle(const char* name)
{
#ifdef __GNUG__
    int status;

    const std::unique_ptr<char, void (*)(void*)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free};

    return status == 0 ? res.get() : name;
#else
    return name;
#endif
}

} // namespace

namespace language::plugins {

bool Type::is_basic_type() const
{
    return false;
}

bool Type::is_internal_type() const
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

bool Type::is_keyword_function_type() const
{
    return false;
}

bool Type::can_convert_into(const Type& other) const
{
    return *this == other;
}

FunctionType::FunctionType(
    const Type& return_type,
    std::vector<const Type*> argument_types)
    : return_type(return_type)
    , argument_types(std::move(argument_types))
{
}

bool FunctionType::operator==(const Type& other) const
{
    if (!other.is_function_type() || other.is_keyword_function_type()) {
        return false;
    }

    const auto& other_casted = static_cast<const FunctionType&>(other);

    return return_type == other_casted.return_type &&
           argument_types == other_casted.argument_types;
}

std::string FunctionType::name() const
{
    return std::format(
        "{} ({:n:s})",
        return_type.name(),
        argument_types |
            std::views::transform([](const Type* t) { return t->name(); }));
}

size_t FunctionType::get_hash() const
{
    // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
    std::size_t seed = argument_types.size() + 1;

    seed ^= return_type.get_hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    for (const auto* t : argument_types) {
        seed ^= t->get_hash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
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

InternalType::InternalType(type_index pointer_type, const string& type_name)
    : pointer_type(pointer_type)
    , type_name(type_name)
{
}

bool InternalType::operator==(const Type& other) const
{
    const InternalType* other_ptr = dynamic_cast<const InternalType*>(&other);
    return other_ptr && pointer_type == other_ptr->pointer_type;
}

bool InternalType::is_internal_type() const
{
    return true;
}

string InternalType::name() const
{
    return type_name;
}

size_t InternalType::get_hash() const
{
    return hash<type_index>()(typeid(InternalType)) ^
           hash<type_index>()(pointer_type);
}

std::type_index InternalType::get_type_index() const
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
        values.push_back(value);
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

int EnumType::get_enum_index(const string& value, Context& context) const
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
    return std::format("{{{:n:s}}}", values);
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
    Context& context)
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

    context.error("Cannot convert {} to {}.", from_type.name(), to_type.name());
}

SymbolType TypeRegistry::SYMBOL_TYPE;
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
        throw downward::utils::CriticalError(
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
        std::make_unique<EnumType>(plugin.get_type(), plugin.get_enum_info()));

    if (!inserted) {
        throw downward::utils::CriticalError(
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

    throw downward::utils::CriticalError(
        "Missing type {}",
        demangle(type.name()));
}
} // namespace language::plugins
