#ifndef LANGUAGE_PLUGINS_INTERNAL_ENUM_DECLARATION_H
#define LANGUAGE_PLUGINS_INTERNAL_ENUM_DECLARATION_H

#include "language/ast/variable_environment.h"

#include "language/plugins/registry_types.h"
#include "language/plugins/type_registry.h"

#include "language/typed_ast/decorated_ast_node.h"
#include "language/typed_ast/variable_declaration.h"

#include <ranges>
#include <string>
#include <type_traits>
#include <typeindex>

namespace language::plugins {

template <typename T>
class EnumeratorDefinition : public parser::VariableDeclaration {
    T value;

public:
    explicit EnumeratorDefinition(std::string variable_name, T value);

    T get_value() const;

    std::unique_ptr<parser::DecoratedASTNode> create_load_node() override;
};

class InternalEnumDeclarationBase {
    std::type_index type;
    std::vector<std::string> enumerator_documentation;

protected:
    InternalEnumDeclarationBase(
        std::type_index type,
        std::vector<std::string> enumerator_documentation);

public:
    virtual ~InternalEnumDeclarationBase() = default;

    std::type_index get_type() const;
    std::string get_class_name() const;

    virtual void
    static_analysis(parser::VariableEnvironment& env, Context& context)
        const = 0;
};

template <typename T>
    requires std::is_enum_v<T>
class InternalEnumDeclaration : public InternalEnumDeclarationBase {
    std::vector<std::unique_ptr<EnumeratorDefinition<T>>> enumerators;

public:
    InternalEnumDeclaration(
        std::initializer_list<std::pair<std::string, std::string>> enum_values)
        : InternalEnumDeclarationBase(
              typeid(T),
              enum_values | std::views::values | std::ranges::to<std::vector>())
    {

        for (std::underlying_type_t<T> i = 0;
             const std::string& name : enum_values | std::views::keys) {
            enumerators.push_back(
                std::make_unique<EnumeratorDefinition<T>>(
                    name,
                    static_cast<T>(i++)));
        }
    }

    void static_analysis(parser::VariableEnvironment& env, Context& context)
        const override
    {
        for (const auto& enumerator : enumerators) {
            env.add_variable(
                context,
                enumerator->variable_name,
                TypeRegistry::instance()->create_enum_type(*this),
                *enumerator);
        }
    }
};

} // namespace language::plugins

#include "language/typed_ast/decorated_enum_constant.h"

namespace language::plugins {

template <typename T>
EnumeratorDefinition<T>::EnumeratorDefinition(
    std::string variable_name,
    T value)
    : VariableDeclaration(std::move(variable_name))
    , value(value)
{
}

template <typename T>
T EnumeratorDefinition<T>::get_value() const
{
    return value;
}

template <typename T>
std::unique_ptr<parser::DecoratedASTNode>
EnumeratorDefinition<T>::create_load_node()
{
    auto node = std::make_unique<parser::DecoratedEnumConstantNode<T>>(*this);
    usages.emplace_back(node.get());
    return node;
}

} // namespace language::plugins

#endif
