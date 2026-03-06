#ifndef LANGUAGE_DECORATED_ENUM_CONSTANT_H
#define LANGUAGE_DECORATED_ENUM_CONSTANT_H

#include "language/typed_ast/decorated_expression_node.h"

namespace language::parser {
template <typename T>
class EnumeratorValue;
}

namespace language::typed_ast {

template <typename T>
class DecoratedEnumConstantNode : public DecoratedExpressionNode {
    const parser::EnumeratorValue<T>& enumerator_definition;

public:
    explicit DecoratedEnumConstantNode(
        const parser::EnumeratorValue<T>& enumerator_definition);

    std::any construct(ConstructContext& context) const override;
};

} // namespace language::parser

#include "language/ast/internal_enum_declaration.h"

namespace language::typed_ast {

template <typename T>
DecoratedEnumConstantNode<T>::DecoratedEnumConstantNode(
    const parser::EnumeratorValue<T>& enumerator_definition)
    : enumerator_definition(enumerator_definition)
{
    enumerator_definition.add_user(this);
}

template <typename T>
std::any DecoratedEnumConstantNode<T>::construct(ConstructContext&) const
{
    return enumerator_definition.get_value();
}

} // namespace language::parser
#endif
