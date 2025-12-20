#ifndef LANGUAGE_DECORATED_FUNCTION_CALL_NODE_H
#define LANGUAGE_DECORATED_FUNCTION_CALL_NODE_H

#include "language/typed_ast/decorated_ast_node.h"

#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class FunctionArgument {
    std::unique_ptr<DecoratedASTNode> value;
    bool is_default;

public:
    FunctionArgument(std::unique_ptr<DecoratedASTNode> value, bool is_default);

    DecoratedASTNode& get_value();
    const DecoratedASTNode& get_value() const;
    bool is_default_argument() const;
};

class DecoratedFunctionCallNode : public DecoratedASTNode {
    std::unique_ptr<DecoratedASTNode> callee;
    std::vector<FunctionArgument> arguments;
    std::string unparsed_config;

public:
    DecoratedFunctionCallNode(
        std::unique_ptr<DecoratedASTNode> callee,
        std::vector<FunctionArgument> arguments,
        const std::string& unparsed_config);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
