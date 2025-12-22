#include "language/ast/variable_environment.h"

#include "language/context.h"
#include "language/plugins/registry.h"

#include <cassert>

using namespace std;

namespace language::parser {

Scope::Scope() = default;

Scope::Scope(std::unique_ptr<Scope> parent)
    : parent(std::move(parent))
{
}

std::unique_ptr<Scope>& Scope::get_parent()
{
    return parent;
}

void Scope::insert(
    Context& context,
    std::pair<std::string, TypedDeclaration> pair)
{
    if (!variables.insert(pair).second) {
        context.error(
            "Variable '{}' is already defined in the current scope.",
            pair.first);
    }
}

bool Scope::insert(std::pair<std::string, TypedDeclaration> pair)
{
    return variables.insert(pair).second;
}

bool Scope::has_variable(const std::string& name) const
{
    return get_typed_declaration(name) != nullptr;
}

TypedDeclaration* Scope::get_typed_declaration(const std::string& name)
{
    if (const auto it = variables.find(name); it != variables.end()) {
        return &it->second;
    }

    return parent ? parent->get_typed_declaration(name) : nullptr;
}

const TypedDeclaration*
Scope::get_typed_declaration(const std::string& name) const
{
    if (const auto it = variables.find(name); it != variables.end()) {
        return &it->second;
    }

    return parent ? parent->get_typed_declaration(name) : nullptr;
}

const plugins::Type& Scope::get_variable_type(const std::string& name)
{
    const auto* t = get_typed_declaration(name);
    assert(t);
    return *t->type;
}

VariableDeclaration& Scope::get_variable_declaration(const std::string& name)
{
    const auto* t = get_typed_declaration(name);
    assert(t);
    return *t->declaration;
}

VariableEnvironment::VariableEnvironment(
    const plugins::Registry& registry,
    Context& context,
    plugins::TypeRegistry& type_registry)
    : registry(registry)
    , scope(std::make_unique<Scope>())
{
    for (const auto& ns = registry.get_global_name_space();
         const auto& enum_decl : ns.get_enum_declarations()) {
        enum_decl->static_analysis(*this, context, type_registry);
    }
}

void VariableEnvironment::add_variable(
    Context& context,
    const std::string& name,
    const plugins::Type& type,
    VariableDeclaration& declaration)
{
    scope->insert(context, {name, TypedDeclaration{&type, &declaration}});
}

bool VariableEnvironment::add_variable(
    const std::string& name,
    const plugins::Type& type,
    VariableDeclaration& declaration)
{
    return scope->insert({name, TypedDeclaration{&type, &declaration}});
}

bool VariableEnvironment::has_variable(const std::string& name) const
{
    return scope->has_variable(name);
}

const plugins::Type&
VariableEnvironment::get_variable_type(const std::string& name) const
{
    return scope->get_variable_type(name);
}

VariableDeclaration&
VariableEnvironment::get_variable_declaration(const std::string& name) const
{
    return scope->get_variable_declaration(name);
}

void VariableEnvironment::enter_scope()
{
    scope = std::make_unique<Scope>(std::move(scope));
}

void VariableEnvironment::leave_scope()
{
    scope = std::move(scope->get_parent());
}

const plugins::Registry& VariableEnvironment::get_registry() const
{
    return registry;
}

} // namespace language::parser
