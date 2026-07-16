#ifndef LANGUAGE_PARSER_DECORATED_EXPRESSION_H
#define LANGUAGE_PARSER_DECORATED_EXPRESSION_H

#include "language/plugins/plugin.h"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class ConstructContext;

struct VariableDefinition;

class DecoratedExpression {
public:
    virtual ~DecoratedExpression() = default;

    std::vector<VariableDefinition> prune_unused_definitions();

    std::any construct() const;

    virtual void prune_unused_definitions(std::vector<VariableDefinition>&) {}

    virtual void remove_variable_usages() {}

    virtual std::any construct(ConstructContext& context) const = 0;

    virtual void
    print(std::ostream& out, std::size_t indent, bool print_default_args)
        const = 0;
};

class VariableNode;

struct VariableDefinition {
    std::string variable_name;
    std::unique_ptr<DecoratedExpression> variable_expression;
    std::vector<VariableNode*> usages;

    VariableDefinition(
        std::string variable_name,
        std::unique_ptr<DecoratedExpression> variable_expression);

    VariableDefinition(VariableDefinition&& other) noexcept;
    VariableDefinition& operator=(VariableDefinition&& other) noexcept;
};

} // namespace language::parser
#endif
