#ifndef LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "language/typed_ast/decorated_ast_node.h"

#include "downward/utils/logging.h"

namespace language::plugins {
class Type;
}

namespace language::parser {

class ConvertNode : public DecoratedASTNode {
    DecoratedASTNodePtr value;
    const plugins::Type& from_type;
    const plugins::Type& to_type;

public:
    ConvertNode(
        DecoratedASTNodePtr value,
        const plugins::Type& from_type,
        const plugins::Type& to_type);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace downward::cli::parser
#endif
