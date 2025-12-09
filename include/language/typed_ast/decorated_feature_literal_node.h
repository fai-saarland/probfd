#ifndef LANGUAGE_DECORATED_FEATURE_LITERAL_NODE_H
#define LANGUAGE_DECORATED_FEATURE_LITERAL_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
}

namespace downward::cli::parser {

class FeatureLiteralNode : public DecoratedASTNode {
    const plugins::InternalFunctionDefinitionBase& feature;

public:
    explicit FeatureLiteralNode(const plugins::InternalFunctionDefinitionBase& feature);

    void remove_variable_usages() override {}

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace downward::cli::parser
#endif
