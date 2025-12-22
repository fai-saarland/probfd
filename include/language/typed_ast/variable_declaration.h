#ifndef LANGUAGE_TYPED_AST_VARIABLE_DECLARATION_H
#define LANGUAGE_TYPED_AST_VARIABLE_DECLARATION_H

#include <deque>
#include <memory>
#include <string>

namespace language::parser {
class DecoratedASTNode;
}

namespace language::parser {

struct VariableDeclaration {
    std::string variable_name;
    std::deque<DecoratedASTNode*> usages;

    explicit VariableDeclaration(std::string variable_name);

    virtual ~VariableDeclaration() = default;

    VariableDeclaration(VariableDeclaration&& other) noexcept = default;
    
    VariableDeclaration&
    operator=(VariableDeclaration&& other) noexcept = default;

    virtual std::unique_ptr<DecoratedASTNode> create_load_node() = 0;
};

} // namespace language::parser
#endif
