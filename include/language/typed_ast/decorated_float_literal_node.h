#ifndef LANGUAGE_DECORATED_FLOAT_LITERAL_NODE_H
#define LANGUAGE_DECORATED_FLOAT_LITERAL_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

namespace downward::cli::parser {

class FloatLiteralNode : public DecoratedASTNode {
    double value;

public:
    explicit FloatLiteralNode(double value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace downward::cli::parser
#endif
