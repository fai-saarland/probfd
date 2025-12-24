#include "language/typed_ast/variable_environment.h"

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

bool Scope::insert(std::string name, TypedVariableDeclaration tdecl)
{
    return variables.emplace(std::move(name), tdecl).second;
}

TypedVariableDeclaration*
Scope::get_variable_declaration(const std::string& name)
{
    if (const auto it = variables.find(name); it != variables.end()) {
        return &it->second;
    }

    return parent ? parent->get_variable_declaration(name) : nullptr;
}

const plugins::Type* Scope::get_type(const std::string& name)
{
    if (const auto it = types.find(name); it != types.end()) {
        return it->second;
    }

    return nullptr;
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

bool VariableEnvironment::add_variable(
    const std::string& name,
    const plugins::Type& type,
    VariableDeclaration& declaration)
{
    return scope->insert(name, TypedVariableDeclaration{&type, &declaration});
}

const plugins::Type*
VariableEnvironment::get_type(const std::string& name) const
{
    return scope->get_type(name);
}

TypedVariableDeclaration*
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
