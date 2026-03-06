#ifndef LANGUAGE_TYPED_AST_NAMESPACE_LEVEL_DECLARATION_H
#define LANGUAGE_TYPED_AST_NAMESPACE_LEVEL_DECLARATION_H

#include "declaration.h"

#include <memory>
#include <ranges>
#include <vector>

namespace language {
class Context;
}

namespace language::parser {
class NamespaceLevelDeclarationVisitor;
} // namespace language::parser

namespace language::typed_ast {
class GlobalEnvironment;
class NamespaceScope;
class TypeRegistry;
} // namespace language::typed_ast

namespace language::parser {

// Declaration that can appear directly within a namespace, i.e, a type
// declaration, another namespace declaration or an enum declaration.
class NamespaceLevelDeclaration : public Declaration {
public:
    using Declaration::Declaration;

    virtual ~NamespaceLevelDeclaration() = default;

    virtual void register_declarations(
        Context& context,
        typed_ast::GlobalEnvironment& env,
        typed_ast::NamespaceScope& scope,
        typed_ast::TypeRegistry& type_registry) const = 0;

    virtual void visit(NamespaceLevelDeclarationVisitor& visitor) = 0;
};

class NamespaceLevelDeclarationList {
protected:
    std::vector<std::unique_ptr<NamespaceLevelDeclaration>> global_declarations;

public:
    template <typename T, typename... Args>
        requires std::constructible_from<T, Args&&...>
    T& emplace_declaration(Args&&... args)
    {
        return static_cast<T&>(*global_declarations.emplace_back(
            std::make_unique<T>(std::forward<Args>(args)...)));
    }

    auto get_declarations(this auto& self)
    {
        return std::views::all(self.global_declarations);
    }
};

} // namespace language::parser

#endif
