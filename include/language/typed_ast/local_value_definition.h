#ifndef LANGUAGE_TYPED_AST_LOCAL_VALUE_DECLARATION_H
#define LANGUAGE_TYPED_AST_LOCAL_VALUE_DECLARATION_H

#include "language/typed_ast/variable_environment.h"

#include <memory>

namespace language::typed_ast {
class DecoratedExpressionNode;
}

namespace language::typed_ast {

struct LocalValueDefinition : StackValue {
    std::unique_ptr<DecoratedExpressionNode> variable_expression;

    LocalValueDefinition(
        std::size_t index,
        std::unique_ptr<DecoratedExpressionNode> variable_expression);

    ~LocalValueDefinition() override;

    LocalValueDefinition(LocalValueDefinition&& other) noexcept;
    LocalValueDefinition& operator=(LocalValueDefinition&& other) noexcept;

    std::unique_ptr<DecoratedExpressionNode> create_load_node() override;
};

} // namespace language::parser

#endif
