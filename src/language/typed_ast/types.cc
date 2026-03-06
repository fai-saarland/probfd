#include "language/typed_ast/types.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

#include "language/utils/demangle.h"

#include <ranges>
#include <typeindex>

using namespace std;

namespace language::typed_ast {

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
        "{}{}",
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

BasicType::BasicType(type_index type)
    : type(type)
{
}

bool BasicType::operator==(const Type& other) const
{
    const auto* other_ptr = dynamic_cast<const BasicType*>(&other);
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
    return utils::demangle(type.name());
}

size_t BasicType::get_hash() const
{
    return hash<type_index>()(type);
}

InternalType::InternalType(type_index pointer_type)
    : type(std::move(pointer_type))
{
}

bool InternalType::operator==(const Type& other) const
{
    const auto* other_ptr = dynamic_cast<const InternalType*>(&other);
    return other_ptr && type == other_ptr->type;
}

bool InternalType::is_internal_type() const
{
    return true;
}

string InternalType::name() const
{
    return utils::demangle(type.name());
}

size_t InternalType::get_hash() const
{
    return hash<type_index>()(typeid(InternalType)) ^ hash<type_index>()(type);
}

std::type_index InternalType::get_type_index() const
{
    return type;
}

ListType::ListType(const Type& nested_type)
    : nested_type(nested_type)
{
}

bool ListType::operator==(const Type& other) const
{
    const auto* other_ptr = dynamic_cast<const ListType*>(&other);
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
    const auto* other_ptr = dynamic_cast<const EmptyListType*>(&other);
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

EnumType::EnumType(type_index type)
    : type(std::move(type))
{
}

bool EnumType::operator==(const Type& other) const
{
    const auto* other_ptr = dynamic_cast<const EnumType*>(&other);
    return other_ptr && type == other_ptr->type;
}

bool EnumType::is_enum_type() const
{
    return true;
}

string EnumType::name() const
{
    return utils::demangle(type.name());
}

size_t EnumType::get_hash() const
{
    return hash<type_index>()(typeid(EnumType)) ^ hash<type_index>()(type);
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
        const int int_value = any_cast<int>(value);
        if (int_value == numeric_limits<int>::max()) {
            return std::any(numeric_limits<double>::infinity());
        } else if (int_value == numeric_limits<int>::min()) {
            return std::any(-numeric_limits<double>::infinity());
        }
        return std::any(static_cast<double>(int_value));
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

} // namespace language::typed_ast
