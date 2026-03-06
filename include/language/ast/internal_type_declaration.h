#ifndef LANGUAGE_AST_INTERNAL_TYPE_DECLARATION_H
#define LANGUAGE_AST_INTERNAL_TYPE_DECLARATION_H

#include "language/typed_ast/variable_environment.h"
#include "namespace_level_declaration.h"

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>

namespace language {
class Context;
}

namespace language::parser {
class NamespaceScope;
}

namespace language::typed_ast {
class TypeRegistry;
}

namespace language::parser {

/*
  The InternalTypeDeclarationBase class contains meta-information for a C++
  type.
*/
class InternalTypeDeclaration : public NamespaceLevelDeclaration {
    std::type_index index;

    /*
      General documentation for the type.
      This is included at the top of the wiki page for this feature type.
    */
    std::string synopsis;

public:
    InternalTypeDeclaration(
        std::type_index index,
        std::string type_identifier,
        std::string synopsis);

    std::type_index get_pointer_type() const;
    std::string get_synopsis() const;

    void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::NamespaceScope& scope,
        typed_ast::TypeRegistry& type_registry) const override;

    void visit(NamespaceLevelDeclarationVisitor& visitor) override;
};

template <typename T>
const InternalTypeDeclaration& insert_type_declaration(
    NamespaceLevelDeclarationList& list,
    std::string name,
    std::string synopsis);

template <typename T>
const InternalTypeDeclaration& insert_shared_type_declaration(
    NamespaceLevelDeclarationList& list,
    std::string name,
    std::string synopsis);

template <template <bool...> typename T, bool... b, typename F1, typename F2>
auto insert_type_declarations(
    NamespaceLevelDeclarationList& list,
    const F1& name,
    const F2& synopsis);

template <template <bool...> typename T, bool... b, typename F1, typename F2>
auto insert_shared_type_declarations(
    NamespaceLevelDeclarationList& list,
    const F1& name,
    const F2& synopsis);

} // namespace language::parser

#include "language/context.h"

#include "../typed_ast/type_registry.h"

#include "language/utils/concepts.h"

namespace language::parser {

template <typename T>
const InternalTypeDeclaration& insert_type_declaration(
    NamespaceLevelDeclarationList& list,
    std::string name,
    std::string synopsis)
{
    return list.emplace_declaration<InternalTypeDeclaration>(
        typeid(T),
        std::move(name),
        std::move(synopsis));
}

template <typename T>
const InternalTypeDeclaration& insert_shared_type_declaration(
    NamespaceLevelDeclarationList& list,
    std::string name,
    std::string synopsis)
{
    return insert_type_declaration<std::shared_ptr<T>>(
        list,
        std::move(name),
        std::move(synopsis));
}

template <template <bool...> typename T, bool... b, typename F1, typename F2>
auto insert_type_declarations(
    NamespaceLevelDeclarationList& list,
    const F1& name,
    const F2& synopsis)
{
    if constexpr (utils::instantiable<T, b...>) {
        return std::tie(
            insert_type_declaration<T<b...>>(
                list,
                name.template operator()<b...>(),
                synopsis.template operator()<b...>()));
    } else {
        return std::tuple_cat(
            insert_type_declarations<T, b..., true>(list, name, synopsis),
            insert_type_declarations<T, b..., false>(list, name, synopsis));
    }
}

template <template <bool...> typename T, bool... b, typename F1, typename F2>
auto insert_shared_type_declarations(
    NamespaceLevelDeclarationList& list,
    const F1& name,
    const F2& synopsis)
{
    if constexpr (utils::instantiable<T, b...>) {
        return std::tie(
            insert_shared_type_declaration<T<b...>>(
                list,
                name.template operator()<b...>(),
                synopsis.template operator()<b...>()));
    } else {
        return std::tuple_cat(
            insert_shared_type_declarations<T, b..., true>(
                list,
                name,
                synopsis),
            insert_shared_type_declarations<T, b..., false>(
                list,
                name,
                synopsis));
    }
}

} // namespace language::parser

#endif
