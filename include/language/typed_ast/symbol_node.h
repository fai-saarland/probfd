#ifndef LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "language/typed_ast/decorated_ast_node.h"

#include <string>

namespace language::parser {

class SymbolNode : public DecoratedASTNode {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
