#ifndef LANGUAGE_DECORATED_LAMBDA_NODE_H
#define LANGUAGE_DECORATED_LAMBDA_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include "language/plugins/types.h"

#include <any>
#include <memory>
#include <vector>

namespace language::parser {
struct VariableDeclaration;
}

namespace language::parser {

class DecoratedLambdaNode : public DecoratedASTNode {
    const plugins::FunctionType& type;
    std::vector<VariableDeclaration> decorated_variable_declarations;
    std::unique_ptr<DecoratedASTNode> nested_value;

public:
    DecoratedLambdaNode(
        const plugins::FunctionType& type,
        std::vector<VariableDeclaration> decorated_variable_declarations,
        std::unique_ptr<DecoratedASTNode> nested_value);

    ~DecoratedLambdaNode() override;

    DecoratedLambdaNode(DecoratedLambdaNode&&) noexcept;

    void
    prune_unused_definitions(std::vector<VariableDefinition>& defs) override;
    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
