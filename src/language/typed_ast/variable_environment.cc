#include "language/typed_ast/variable_environment.h"

#include "language/ast/compilation_context.h"
#include "language/ast/qualified_name.h"

#include "language/context.h"

using namespace std;

namespace language::typed_ast {

bool BlockScope::insert(std::string name, TypedValue tdecl)
{
    return variables.emplace(std::move(name), tdecl).second;
}

const TypedValue*
BlockScope::get_variable_declaration(std::string_view name) const
{
    if (const auto it = variables.find(name); it != variables.end()) {
        return &it->second;
    }

    return nullptr;
}

std::size_t BlockScope::get_num_variables() const
{
    return variables.size();
}

void GlobalScope::insert_declaration(
    std::string name,
    const Type& type,
    Value& declaration,
    const Context& context)
{
    if (const auto [it, inserted] =
            variables.emplace(name, TypedValue{&type, &declaration});
        !inserted) {
        std::visit(
            utils::overload{
                [&](const TypedValue&) {
                    context.error(
                        "Object '{}' has already been declared as a different "
                        "object.",
                        name);
                },
                [&](const std::vector<TypedFunctionValue>&) {
                    context.error(
                        "Object '{}' has already been declared as a function.");
                }},
            it->second);
    }
}

void GlobalScope::insert_declaration(
    std::string name,
    const Type& type,
    FunctionValue& declaration,
    const Context& context)
{
    if (const auto [it, inserted] =
            variables.emplace(name, TypedValue{&type, &declaration});
        !inserted) {
        std::visit(
            utils::overload{
                [&](const TypedValue&) {
                    context.error(
                        "Function '{}' has already been declared as an object.",
                        name);
                },
                [&](std::vector<TypedFunctionValue>&) {
                    context.error("Function overloading not supported!");
                }},
            it->second);
    }
}

std::variant<std::monostate, TypedValue, const std::vector<TypedFunctionValue>*>
GlobalScope::get_variable_declaration(std::string_view name) const
{
    using R = std::variant<
        std::monostate,
        TypedValue,
        const std::vector<TypedFunctionValue>*>;

    if (const auto it = variables.find(name); it != variables.end()) {
        return std::visit(
            utils::overload{
                [](const TypedValue& object) -> R { return object; },
                [](const std::vector<TypedFunctionValue>& object) -> R {
                    return &object;
                }},
            it->second);
    }

    return {};
}

NamespaceScope& NamespaceScope::create_or_get_namespace(
    std::string_view name,
    const Context& context)
{
    const auto it = nested_scopes.find(name);

    if (it != nested_scopes.end()) {
        return std::visit(
            utils::overload{
                [&](const std::unique_ptr<NamespaceScope>& object)
                    -> NamespaceScope& { return *object; },
                [&](const std::unique_ptr<TypeScope>&) -> NamespaceScope& {
                    context.error(
                        "Namespace '{}' has already been declared as a class.",
                        name);
                },
            },
            it->second);
    }

    auto n = std::make_unique<NamespaceScope>();
    auto* ptr = n.get();
    nested_scopes.emplace(name, std::move(n)).first->second;
    return *ptr;
}

TypeScope& NamespaceScope::create_or_get_type(
    std::string_view name,
    const Type& type,
    const Context& context)
{
    const auto it = nested_scopes.find(name);

    if (it != nested_scopes.end()) {
        return std::visit(
            utils::overload{
                [&](const std::unique_ptr<NamespaceScope>&) -> TypeScope& {
                    context.error(
                        "Type '{}' has already been declared as a namespace.",
                        name);
                },
                [](const std::unique_ptr<TypeScope>& ptr) -> TypeScope& {
                    return *ptr;
                }},
            it->second);
    }

    auto n = std::make_unique<TypeScope>(type);
    auto* ptr = n.get();
    nested_scopes.emplace(name, std::move(n)).first->second;
    return *ptr;
}

std::variant<std::monostate, const NamespaceScope*, const TypeScope*>
NamespaceScope::get_nested_scope(std::string_view name) const
{
    if (const auto it = nested_scopes.find(name); it != nested_scopes.end()) {
        return std::visit(
            [](auto& p) -> std::variant<
                            std::monostate,
                            const NamespaceScope*,
                            const TypeScope*> { return p.get(); },
            it->second);
    }

    return {};
}

TypeScope::TypeScope(const Type& type)
    : type(type)
{
}

const TypeScope* TypeScope::get_nested_scope(std::string_view name) const
{
    if (const auto it = nested_scopes.find(name); it != nested_scopes.end()) {
        return it->second.get();
    }

    return nullptr;
}

const Type& TypeScope::get_associated_type() const
{
    return type;
}

bool LocalEnvironment::add_variable(
    const std::string& name,
    const Type& type,
    Value& declaration)
{
    const bool ins =
        local_scopes.back().insert(name, TypedValue{&type, &declaration});

    if (ins) { ++num_local_variables; }

    return ins;
}

const TypedValue*
LocalEnvironment::get_variable_declaration(std::string_view name) const
{
    for (const auto& scope : local_scopes | std::views::reverse) {
        if (const auto* decl = scope.get_variable_declaration(name)) {
            return decl;
        }
    }

    return nullptr;
}

void LocalEnvironment::enter_scope()
{
    local_scopes.emplace_back();
}

void LocalEnvironment::leave_scope()
{
    num_local_variables -= local_scopes.back().get_num_variables();
    local_scopes.pop_back();
}

std::size_t LocalEnvironment::get_num_local_variables() const
{
    return num_local_variables;
}

GlobalEnvironment::GlobalEnvironment(
    parser::CompilationContext& ccontext,
    Context& context,
    TypeRegistry& type_registry)
    : global_namespace(std::make_unique<NamespaceScope>())
{
    ccontext.register_declarations(context, *this, type_registry);
}

const Type& GlobalEnvironment::get_type(
    const parser::QualifiedName& name,
    const Context& context) const
{
    const GlobalScope* active_scope = &get_global_namespace();

    auto it = name.parts.begin();
    const auto end = name.parts.end();

    for (; it != end; ++it) {
        const auto* scope = static_cast<const NamespaceScope*>(active_scope);
        const auto nested_scope_var = scope->get_nested_scope(*it);

        const bool stop = std::visit(
            utils::overload{
                [&](std::monostate) -> bool {
                    context.error(
                        "Could not resolve type '{}'",
                        parser::NamespaceFormatted{std::ranges::subrange(
                            name.qualification_begin(),
                            std::next(it))});
                },
                [&](const NamespaceScope* nested_scope) {
                    active_scope = nested_scope;
                    return false;
                },
                [&](const TypeScope* nested_scope) {
                    active_scope = nested_scope;
                    return true;
                },
            },
            nested_scope_var);

        if (stop) goto type_found;
    }

    context.error(
        "Symbol '{}' designates a namespace, but expected a type.",
        name);

type_found:

    for (auto it2 = std::next(it); it2 != end; ++it2) {
        active_scope =
            static_cast<const TypeScope*>(active_scope)->get_nested_scope(*it2);

        if (!active_scope) {
            context.error(
                "Could not resolve type '{}'",
                parser::NamespaceFormatted{std::ranges::subrange(
                    name.qualification_begin(),
                    std::next(it2))});
        }
    }

    return static_cast<const TypeScope*>(active_scope)->get_associated_type();
}

std::variant<TypedValue, const std::vector<TypedFunctionValue>*>
GlobalEnvironment::get_variable_declaration(
    const LocalEnvironment& local_environment,
    const parser::QualifiedName& name,
    const Context& context) const
{
    if (const auto* decl =
            local_environment.get_variable_declaration(name.name())) {
        return TypedValue{decl->type, decl->declaration};
    }

    const GlobalScope* active_scope = &get_global_namespace();

    auto it = name.qualification_begin();
    const auto end = name.qualification_end();

    for (; it != end; ++it) {
        const auto* scope = static_cast<const NamespaceScope*>(active_scope);
        const auto nested_scope_var = scope->get_nested_scope(*it);

        const bool stop = std::visit(
            utils::overload{
                [&](std::monostate) -> bool {
                    context.error(
                        "Could not resolve scope '{}'",
                        parser::NamespaceFormatted{std::ranges::subrange(
                            name.qualification_begin(),
                            std::next(it))});
                },
                [&](const NamespaceScope* nested_scope) {
                    active_scope = nested_scope;
                    return false;
                },
                [&](const TypeScope* nested_scope) {
                    active_scope = nested_scope;
                    return true;
                },
            },
            nested_scope_var);

        if (stop) {
            for (auto it2 = std::next(it); it2 != end; ++it2) {
                active_scope = static_cast<const TypeScope*>(active_scope)
                                   ->get_nested_scope(*it2);

                if (!active_scope) {
                    context.error(
                        "Could not resolve scope '{}'",
                        parser::NamespaceFormatted{std::ranges::subrange(
                            name.qualification_begin(),
                            std::next(it2))});
                }
            }

            break;
        }
    }

    const auto decl = active_scope->get_variable_declaration(name.name());

    using R = std::variant<TypedValue, const std::vector<TypedFunctionValue>*>;

    return std::visit(
        utils::overload{
            [&](std::monostate) -> R {
                context.error(
                    "Could not resolve symbol '{}' within scope '{}'.",
                    name.name(),
                    parser::NamespaceFormatted{name.qualification_prefix()});
            },
            []<typename T>(const T& t) -> R { return t; }},
        decl);
}

} // namespace language::typed_ast
