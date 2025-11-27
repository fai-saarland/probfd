#ifndef PARSER_LIST_LITERAL_NODE_H
#define PARSER_LIST_LITERAL_NODE_H

#include "downward/cli/parser/ast/expression_node.h"

#include <memory>
#include <string>
#include <vector>

namespace downward::cli::parser {

class ListNode : public ASTNode {
    std::vector<ASTNodePtr> elements;

public:
    explicit ListNode(std::vector<ASTNodePtr>&& elements);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

} // namespace downward::cli::parser

#endif
