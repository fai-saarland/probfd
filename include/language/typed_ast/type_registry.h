#ifndef LANGUAGE_TYPED_AST_TYPE_REGISTRY_H
#define LANGUAGE_TYPED_AST_TYPE_REGISTRY_H

#include "language/typed_ast/types.h"

#include <algorithm>
#include <any>
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace language {
class Context;
}

namespace language::parser {
class InternalEnumDeclarationBase;
class InternalTypeDeclaration;
} // namespace language::parser

namespace language::typed_ast {

class TypeRegistry {
    template <typename T>
    struct TypeOf {
        static const Type& value(TypeRegistry& registry);
    };

    template <typename T>
    struct TypeOf<std::vector<T>> {
        static const ListType& value(TypeRegistry& registry);
    };

    template <typename R, typename... Args>
    struct TypeOf<R(Args...)> {
        static const FunctionType& value(TypeRegistry& registry);
    };

    struct SemanticHash {
        size_t operator()(const auto& t) const
        {
            if (!t) { return 0; }
            return t->get_hash();
        }
    };

    struct SemanticEqual {
        size_t operator()(const auto& t1, const auto& t2) const
        {
            if (!t1 || !t2) { return t1 == t2; }
            return t1->operator==(*t2);
        }
    };

    std::unordered_map<std::type_index, std::unique_ptr<Type>> registered_types;

    std::unordered_map<
        const Type*,
        std::unique_ptr<ListType>,
        SemanticHash,
        SemanticEqual>
        registered_list_types;

    std::unordered_set<
        std::unique_ptr<FunctionType>,
        SemanticHash,
        SemanticEqual>
        registered_function_types;

public:
    static EmptyListType EMPTY_LIST_TYPE;

    TypeRegistry();

    const InternalType&
    create_feature_type(const parser::InternalTypeDeclaration& plugin);

    const EnumType&
    create_enum_type(const parser::InternalEnumDeclarationBase& plugin);

    const ListType& create_list_type(const Type& element_type);
    const FunctionType& create_function_type(
        const Type& return_type,
        std::vector<const Type*> arg_types);

    template <typename T>
    const Type& get_type();

    const FunctionType& get_function_type(
        std::type_index return_type,
        std::vector<std::type_index> arg_types);

private:
    template <typename T>
    void insert_basic_type();

    const Type& get_nonlist_type(std::type_index type) const;
};

} // namespace language::typed_ast

namespace language::typed_ast {

template <typename T>
const Type& TypeRegistry::TypeOf<T>::value(TypeRegistry& registry)
{
    return registry.get_nonlist_type(typeid(T));
}

template <typename T>
const ListType&
TypeRegistry::TypeOf<std::vector<T>>::value(TypeRegistry& registry)
{
    return registry.create_list_type(registry.get_type<T>());
}

template <typename R, typename... Args>
const FunctionType&
TypeRegistry::TypeOf<R(Args...)>::value(TypeRegistry& registry)
{
    return registry.create_function_type(
        registry.get_type<R>(),
        std::vector<const Type*>{&registry.get_type<Args>()...});
}

template <typename T>
const Type& TypeRegistry::get_type()
{
    return TypeOf<T>::value(*this);
}

inline const FunctionType& TypeRegistry::get_function_type(
    std::type_index return_type,
    std::vector<std::type_index> arg_types)
{
    return create_function_type(
        get_nonlist_type(return_type),
        arg_types | std::views::transform([&](const auto& index) {
            return &get_nonlist_type(index);
        }) | std::ranges::to<std::vector>());
}

} // namespace language::typed_ast

#endif
