#include "language/typed_ast/variable_environment.h"

#include "language/context.h"
#include "language/plugins/registry.h"

#include <cassert>

using namespace std;

namespace language::parser {

bool Scope::insert(std::string name, TypedVariableDeclaration tdecl)
{
    return variables.emplace(std::move(name), tdecl).second;
}

const TypedVariableDeclaration*
Scope::get_variable_declaration(const std::string& name) const
{
    if (const auto it = variables.find(name); it != variables.end()) {
        return &it->second;
    }

    return nullptr;
}

const plugins::Type* Scope::get_type(const std::string& name) const
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
    , scopes(1, Scope())
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
    return scopes.back().insert(
        name,
        TypedVariableDeclaration{&type, &declaration});
}

const plugins::Type*
VariableEnvironment::get_type(const std::string& name) const
{
    for (const auto& scope : scopes | std::views::reverse) {
        if (const auto* type = scope.get_type(name)) { return type; }
    }

    return nullptr;
}

const TypedVariableDeclaration*
VariableEnvironment::get_variable_declaration(const std::string& name) const
{
    for (const auto& scope : scopes | std::views::reverse) {
        if (const auto* decl = scope.get_variable_declaration(name)) {
            return decl;
        }
    }

    return nullptr;
}

void VariableEnvironment::enter_scope()
{
    scopes.emplace_back();
}

void VariableEnvironment::leave_scope()
{
    scopes.pop_back();
}

const plugins::Registry& VariableEnvironment::get_registry() const
{
    return registry;
}

} // namespace language::parser
