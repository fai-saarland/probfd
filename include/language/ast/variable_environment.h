#ifndef LANGUAGE_AST_VARIABLE_ENVIRONMENT_H
#define LANGUAGE_AST_VARIABLE_ENVIRONMENT_H

#include <memory>
#include <string>
#include <unordered_map>

namespace language {
class Context;
} // namespace downward::utils

namespace language::plugins {
class Registry;
class Type;
} // namespace language::plugins

namespace language::parser {

struct VariableDeclaration;

struct TypedDeclaration {
    const plugins::Type* type;
    VariableDeclaration* declaration;
};

class Scope {
    std::unique_ptr<Scope> parent = nullptr;
    std::unordered_map<std::string, TypedDeclaration> variables;

public:
    Scope();

    explicit Scope(std::unique_ptr<Scope> parent);

    std::unique_ptr<Scope>& get_parent();

    void insert(
        Context& context,
        std::pair<std::string, TypedDeclaration> pair);

    bool insert(std::pair<std::string, TypedDeclaration> pair);

    bool has_variable(const std::string& name) const;

    TypedDeclaration* get_typed_declaration(const std::string& name);

    const TypedDeclaration*
    get_typed_declaration(const std::string& name) const;

    const plugins::Type& get_variable_type(const std::string& name);

    VariableDeclaration& get_variable_declaration(const std::string& name);
};

class VariableEnvironment {
    const plugins::Registry& registry;
    std::unique_ptr<Scope> scope;

public:
    explicit VariableEnvironment(const plugins::Registry& registry);

    void add_variable(
        Context& context,
        const std::string& name,
        const plugins::Type& type,
        VariableDeclaration& declaration);

    bool add_variable(
        const std::string& name,
        const plugins::Type& type,
        VariableDeclaration& declaration);

    bool has_variable(const std::string& name) const;

    const plugins::Type& get_variable_type(const std::string& name) const;

    VariableDeclaration& get_variable_definition(const std::string& name) const;

    void enter_scope();

    void leave_scope();

    const plugins::Registry& get_registry() const;
};

} // namespace language::parser

#endif
