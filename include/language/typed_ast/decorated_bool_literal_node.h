#ifndef LANGUAGE_DECORATED_BOOL_LITERAL_NODE_H
#define LANGUAGE_DECORATED_BOOL_LITERAL_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

namespace language::parser {

class BoolLiteralNode : public DecoratedASTNode {
    bool value;

public:
    explicit BoolLiteralNode(bool value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
