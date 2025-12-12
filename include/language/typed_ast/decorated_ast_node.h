#ifndef LANGUAGE_DECORATED_AST_NODE_H
#define LANGUAGE_DECORATED_AST_NODE_H

#include <any>
#include <cstddef>
#include <iostream>
#include <vector>

namespace language::parser {
class ConstructContext;
struct VariableDefinition;
} // namespace downward::cli::parser

namespace language::parser {

class DecoratedASTNode {
public:
    virtual ~DecoratedASTNode() = default;

    virtual void prune_unused_definitions(std::vector<VariableDefinition>&) {}

    virtual void remove_variable_usages() {}

    virtual std::any construct(ConstructContext& context) const = 0;
    virtual void
    print(std::ostream& out, std::size_t indent, bool print_default_args)
        const = 0;

    std::vector<VariableDefinition> prune_unused_definitions();
    std::any construct() const;
};

} // namespace downward::cli::parser
#endif
