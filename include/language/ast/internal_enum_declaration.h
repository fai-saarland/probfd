#ifndef LANGUAGE_AST_INTERNAL_ENUM_DECLARATION_H
#define LANGUAGE_AST_INTERNAL_ENUM_DECLARATION_H

#include "language/ast/declaration.h"
#include "language/ast/namespace_level_declaration.h"

#include "language/typed_ast/decorated_expression_node.h"
#include "language/typed_ast/type_registry.h"
#include "language/typed_ast/variable_environment.h"

#include <ranges>
#include <string>
#include <type_traits>
#include <typeindex>

namespace language::parser {

class InternalEnumDeclarationBase : public NamespaceLevelDeclaration {
protected:
    std::type_index type;
    std::vector<std::string> enumerator_names;
    std::vector<std::string> enumerator_documentation;

    std::string title;
    std::string synopsis;

    InternalEnumDeclarationBase(
        std::type_index type,
        std::string type_identifier,
        std::vector<std::string> enumerator_names,
        std::vector<std::string> enumerator_documentation);

public:
    void document_title(const std::string& title);

    void document_synopsis(const std::string& synopsis);

    std::type_index get_type() const;

    auto get_enumerator_names() const
    {
        return enumerator_names | std::views::all;
    }

    auto get_enumerator_docs() const
    {
        return enumerator_documentation | std::views::all;
    }

    const std::string& get_title() const;

    const std::string& get_synopsis() const;

    void visit(NamespaceLevelDeclarationVisitor& visitor) override;
};

template <typename T>
class EnumeratorValue : public typed_ast::Value {
    T value;

public:
    explicit EnumeratorValue(T value);

    T get_value() const;

    std::unique_ptr<typed_ast::DecoratedExpressionNode>
    create_load_node() override;
};

template <typename T>
class EnumeratorDefinition {
    std::unique_ptr<EnumeratorValue<T>> enumerator_value;

public:
    explicit EnumeratorDefinition(T value);

    EnumeratorValue<T>& get_enumerator_value() const;
};

template <typename T>
    requires std::is_enum_v<T>
class InternalEnumDeclaration : public InternalEnumDeclarationBase {
    std::vector<EnumeratorDefinition<T>> enumerators;

public:
    InternalEnumDeclaration(
        std::string type_identifier,
        std::initializer_list<std::pair<std::string, std::string>> enum_values);

    void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::NamespaceScope& scope,
        typed_ast::TypeRegistry& type_registry) const override;
};

template <typename T>
auto& insert_enum_declaration(
    NamespaceLevelDeclarationList& list,
    std::string type_identifier,
    std::initializer_list<std::pair<std::string, std::string>> enum_values);

} // namespace language::parser

#include "language/context.h"

#include "language/typed_ast/decorated_enum_constant.h"

namespace language::parser {

template <typename T>
EnumeratorValue<T>::EnumeratorValue(T value)
    : value(value)
{
}

template <typename T>
T EnumeratorValue<T>::get_value() const
{
    return value;
}

template <typename T>
std::unique_ptr<typed_ast::DecoratedExpressionNode>
EnumeratorValue<T>::create_load_node()
{
    return std::make_unique<typed_ast::DecoratedEnumConstantNode<T>>(*this);
}

template <typename T>
EnumeratorDefinition<T>::EnumeratorDefinition(T value)
    : enumerator_value(std::make_unique<EnumeratorValue<T>>(value))
{
}

template <typename T>
EnumeratorValue<T>& EnumeratorDefinition<T>::get_enumerator_value() const
{
    return *enumerator_value;
}

template <typename T>
    requires std::is_enum_v<T>
InternalEnumDeclaration<T>::InternalEnumDeclaration(
    std::string type_identifier,
    std::initializer_list<std::pair<std::string, std::string>> enum_values)
    : InternalEnumDeclarationBase(
          typeid(T),
          std::move(type_identifier),
          enum_values | std::views::keys | std::ranges::to<std::vector>(),
          enum_values | std::views::values | std::ranges::to<std::vector>())
{
    for (std::size_t i = 0; i != enum_values.size(); ++i) {
        enumerators.emplace_back(
            static_cast<T>(static_cast<std::underlying_type_t<T>>(i)));
    }
}

template <typename T>
    requires std::is_enum_v<T>
void InternalEnumDeclaration<T>::register_declarations(
    Context& context,
    typed_ast::GlobalEnvironment&,
    typed_ast::NamespaceScope& scope,
    typed_ast::TypeRegistry& type_registry) const
{
    const typed_ast::Type& enum_type = type_registry.create_enum_type(*this);

    typed_ast::TypeScope& nested_scope =
        scope.create_or_get_type(this->identifier, enum_type, context);

    for (const auto& [enumerator_identifier, enumerator] :
         std::views::zip(enumerator_names, enumerators)) {
        nested_scope.insert_declaration(
            enumerator_identifier,
            enum_type,
            enumerator.get_enumerator_value(),
            context);
    }
}

template <typename T>
auto& insert_enum_declaration(
    NamespaceLevelDeclarationList& list,
    std::string type_identifier,
    std::initializer_list<std::pair<std::string, std::string>> enum_values)
{
    return list.emplace_declaration<InternalEnumDeclaration<T>>(
        std::move(type_identifier),
        enum_values);
}

} // namespace language::parser

#endif
