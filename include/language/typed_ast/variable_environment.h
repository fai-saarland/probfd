#ifndef LANGUAGE_TYPED_AST_VARIABLE_ENVIRONMENT_H
#define LANGUAGE_TYPED_AST_VARIABLE_ENVIRONMENT_H

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>

namespace language {
class Context;
} // namespace language

namespace language::plugins {
class Registry;
class Type;
class TypeRegistry;
} // namespace language::plugins

namespace language::parser {

struct VariableDeclaration;

struct TypedVariableDeclaration {
    const plugins::Type* type;
    VariableDeclaration* declaration;
};

class Scope {
    std::unordered_map<std::string, TypedVariableDeclaration> variables;
    std::unordered_map<std::string, const plugins::Type*> types;

public:
    bool insert(std::string name, TypedVariableDeclaration tdecl);

    const TypedVariableDeclaration*
    get_variable_declaration(const std::string& name) const;

    const plugins::Type* get_type(const std::string& name) const;
};

class VariableEnvironment {
    const plugins::Registry& registry;
    std::deque<Scope> scopes; // top-level scope first

public:
    explicit VariableEnvironment(
        const plugins::Registry& registry,
        Context& context,
        plugins::TypeRegistry& type_registry);

    bool add_variable(
        const std::string& name,
        const plugins::Type& type,
        VariableDeclaration& declaration);

    const plugins::Type* get_type(const std::string& name) const;

    const TypedVariableDeclaration*
    get_variable_declaration(const std::string& name) const;

    void enter_scope();

    void leave_scope();

    const plugins::Registry& get_registry() const;
};

} // namespace language::parser

#endif
