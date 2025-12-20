#ifndef LANGUAGE_TYPED_AST_VARIABLE_DEFINITION_H
#define LANGUAGE_TYPED_AST_VARIABLE_DEFINITION_H

#include <memory>
#include <string>
#include <vector>

namespace language::parser {
class DecoratedASTNode;
class VariableNode;
}

namespace language::parser {

struct VariableDeclaration {
    std::string variable_name;
    std::vector<VariableNode*> usages;

    explicit VariableDeclaration(std::string variable_name);

    VariableDeclaration(VariableDeclaration&& other) noexcept;
    VariableDeclaration& operator=(VariableDeclaration&& other) noexcept;
};

struct VariableDefinition : VariableDeclaration {
    std::unique_ptr<DecoratedASTNode> variable_expression;

    VariableDefinition(
        std::string variable_name,
        std::unique_ptr<DecoratedASTNode> variable_expression);

    ~VariableDefinition();

    VariableDefinition(VariableDefinition&& other) noexcept;
    VariableDefinition& operator=(VariableDefinition&& other) noexcept;
};

} // namespace language::parser
#endif
