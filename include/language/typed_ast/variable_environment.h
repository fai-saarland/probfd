#ifndef LANGUAGE_TYPED_AST_VARIABLE_ENVIRONMENT_H
#define LANGUAGE_TYPED_AST_VARIABLE_ENVIRONMENT_H

#include "language/ast/direct_function_call_node.h"

#include "language/typed_ast/value.h"

#include "language/utils/string_map.h"

#include <deque>
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace language::parser {
class CompilationContext;
struct QualifiedName;
} // namespace language::parser

namespace language {
class Context;
} // namespace language

namespace language::typed_ast {
class DecoratedExpressionNode;
class FunctionType;
class Type;
class TypeRegistry;
} // namespace language::typed_ast

namespace language::typed_ast {

class BlockScope {
    utils::UnorderedStringMap<TypedValue> variables;

public:
    bool insert(std::string name, TypedValue tdecl);

    const TypedValue* get_variable_declaration(std::string_view name) const;

    std::size_t get_num_variables() const;
};

// Either a type or namespace scope
class GlobalScope {
    utils::UnorderedStringMap<
        std::variant<TypedValue, std::vector<TypedFunctionValue>>>
        variables;

public:
    void insert_declaration(
        std::string name,
        const Type& type,
        Value& declaration,
        const Context& context);

    void insert_declaration(
        std::string name,
        const Type& type,
        FunctionValue& declaration,
        const Context& context);

    std::variant<
        std::monostate,
        TypedValue,
        const std::vector<TypedFunctionValue>*>
    get_variable_declaration(std::string_view name) const;
};

class NamespaceScope;
class TypeScope;

class NamespaceScope final : public GlobalScope {
    utils::UnorderedStringMap<std::variant<
        std::unique_ptr<NamespaceScope>,
        std::unique_ptr<TypeScope>>>
        nested_scopes;

public:
    NamespaceScope&
    create_or_get_namespace(std::string_view name, const Context& context);

    TypeScope& create_or_get_type(
        std::string_view name,
        const Type& type,
        const Context& context);

    std::variant<std::monostate, const NamespaceScope*, const TypeScope*>
    get_nested_scope(std::string_view name) const;
};

class TypeScope final : public GlobalScope {
    // The type associated with this scope.
    const Type& type;

    // A type scope can only have nested scopes of nested types.
    utils::UnorderedStringMap<std::unique_ptr<TypeScope>> nested_scopes;

public:
    explicit TypeScope(const Type& type);

    const TypeScope* get_nested_scope(std::string_view name) const;

    const Type& get_associated_type() const;
};

class LocalEnvironment {
    // top-level scope first
    std::deque<BlockScope> local_scopes = {1, BlockScope{}};

    std::size_t num_local_variables = 0;

public:
    bool
    add_variable(const std::string& name, const Type& type, Value& declaration);

    const TypedValue* get_variable_declaration(std::string_view name) const;

    void enter_scope();
    void leave_scope();

    std::size_t get_num_local_variables() const;
};

class GlobalEnvironment {
    std::vector<std::unique_ptr<GlobalValue>> global_values;
    std::unique_ptr<NamespaceScope> global_namespace;

public:
    explicit GlobalEnvironment(
        parser::CompilationContext& ccontext,
        Context& context,
        TypeRegistry& type_registry);

    template <std::derived_from<GlobalValue> T, typename... Args>
        requires std::constructible_from<T, Args&&...>
    T& emplace_global_value(Args&&... args)
    {
        return static_cast<T&>(*global_values.emplace_back(
            std::make_unique<T>(std::forward<Args>(args)...)));
    }

    auto& get_global_namespace(this auto& self)
    {
        return *self.global_namespace;
    }

    const Type&
    get_type(const parser::QualifiedName& name, const Context& context) const;

    std::variant<TypedValue, const std::vector<TypedFunctionValue>*>
    get_variable_declaration(
        const LocalEnvironment& local_environment,
        const parser::QualifiedName& name,
        const Context& context) const;
};

} // namespace language::typed_ast

#endif
