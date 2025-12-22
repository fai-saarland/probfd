#ifndef LANGUAGE_DECORATED_ENUM_CONSTANT_H
#define LANGUAGE_DECORATED_ENUM_CONSTANT_H

#include "language/typed_ast/decorated_ast_node.h"

namespace language::plugins {
template <typename T>
class EnumeratorDefinition;
}

namespace language::parser {

template <typename T>
class DecoratedEnumConstantNode : public DecoratedASTNode {
    const plugins::EnumeratorDefinition<T>& enumerator_definition;

public:
    explicit DecoratedEnumConstantNode(
        const plugins::EnumeratorDefinition<T>& enumerator_definition);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream&, std::size_t, bool) const override;
};

} // namespace language::parser

#include "language/plugins/internal_enum_declaration.h"

namespace language::parser {

template <typename T>
DecoratedEnumConstantNode<T>::DecoratedEnumConstantNode(
    const plugins::EnumeratorDefinition<T>& enumerator_definition)
    : enumerator_definition(enumerator_definition)
{
}

template <typename T>
std::any DecoratedEnumConstantNode<T>::construct(ConstructContext&) const
{
    return std::any(enumerator_definition.get_value());
}

template <typename T>
void DecoratedEnumConstantNode<T>::print(std::ostream&, std::size_t, bool) const
{
}

} // namespace language::parser
#endif
