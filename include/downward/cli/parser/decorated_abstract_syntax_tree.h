#ifndef PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/logging.h"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace downward::plugins {
class Options;
}

namespace downward::cli::parser {

class ConstructContext;

class DecoratedASTNode;
using DecoratedASTNodePtr = std::unique_ptr<DecoratedASTNode>;

class VariableNode;

struct VariableDeclaration {
    std::string variable_name;
    std::vector<VariableNode*> usages;

    explicit VariableDeclaration(std::string variable_name);

    VariableDeclaration(VariableDeclaration&& other) noexcept;
    VariableDeclaration& operator=(VariableDeclaration&& other) noexcept;
};

struct VariableDefinition : VariableDeclaration {
    DecoratedASTNodePtr variable_expression;

    VariableDefinition(
        std::string variable_name,
        DecoratedASTNodePtr variable_expression);

    VariableDefinition(VariableDefinition&& other) noexcept;
    VariableDefinition& operator=(VariableDefinition&& other) noexcept;
};

class DecoratedASTNode {
public:
    virtual ~DecoratedASTNode() = default;

    std::vector<VariableDefinition> prune_unused_definitions();
    std::any construct() const;

    virtual void prune_unused_definitions(std::vector<VariableDefinition>&) {}

    virtual void remove_variable_usages() {}

    virtual std::any construct(ConstructContext& context) const = 0;
    virtual void
    print(std::ostream& out, std::size_t indent, bool print_default_args)
        const = 0;
};

class FunctionArgument {
    DecoratedASTNodePtr value;
    bool is_default;

public:
    FunctionArgument(DecoratedASTNodePtr value, bool is_default);

    DecoratedASTNode& get_value();
    const DecoratedASTNode& get_value() const;
    bool is_default_argument() const;
};

class DecoratedLetNode : public DecoratedASTNode {
    std::vector<VariableDefinition> decorated_variable_definitions;
    DecoratedASTNodePtr nested_value;

public:
    DecoratedLetNode(
        std::vector<VariableDefinition> decorated_variable_definitions,
        DecoratedASTNodePtr nested_value);

    void
    prune_unused_definitions(std::vector<VariableDefinition>& defs) override;
    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedLambdaNode : public DecoratedASTNode {
    const plugins::FunctionType& type;
    std::vector<VariableDeclaration> decorated_variable_declarations;
    DecoratedASTNodePtr nested_value;

public:
    DecoratedLambdaNode(
        const plugins::FunctionType& type,
        std::vector<VariableDeclaration> decorated_variable_declarations,
        DecoratedASTNodePtr nested_value);

    void
    prune_unused_definitions(std::vector<VariableDefinition>& defs) override;
    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedFunctionCallNode : public DecoratedASTNode {
    DecoratedASTNodePtr callee;
    std::vector<FunctionArgument> arguments;
    std::string unparsed_config;

public:
    DecoratedFunctionCallNode(
        DecoratedASTNodePtr callee,
        std::vector<FunctionArgument> arguments,
        const std::string& unparsed_config);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedListNode : public DecoratedASTNode {
    std::vector<DecoratedASTNodePtr> elements;

public:
    explicit DecoratedListNode(std::vector<DecoratedASTNodePtr>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;

    const std::vector<DecoratedASTNodePtr>& get_elements() const
    {
        return elements;
    }
};

class VariableNode : public DecoratedASTNode {
    friend VariableDeclaration;

    VariableDeclaration* declaration;

public:
    explicit VariableNode(VariableDeclaration& declaration);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class FeatureLiteralNode : public DecoratedASTNode {
    const plugins::InternalFunctionDefinitionBase& feature;

public:
    explicit FeatureLiteralNode(const plugins::InternalFunctionDefinitionBase& feature);

    void remove_variable_usages() override {}

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

template <typename T>
    requires std::same_as<T, int> || std::same_as<T, double>
class DecoratedUnaryExpressionNode : public DecoratedASTNode {
    DecoratedASTNodePtr nested_expr;
    TokenType token_type;

public:
    DecoratedUnaryExpressionNode(
        DecoratedASTNodePtr nested_expr,
        TokenType token_type);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

extern template class DecoratedUnaryExpressionNode<int>;
extern template class DecoratedUnaryExpressionNode<double>;

class BoolLiteralNode : public DecoratedASTNode {
    bool value;

public:
    explicit BoolLiteralNode(bool value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class StringLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class IntLiteralNode : public DecoratedASTNode {
    int value;

public:
    explicit IntLiteralNode(int value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class FloatLiteralNode : public DecoratedASTNode {
    double value;

public:
    explicit FloatLiteralNode(double value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class SymbolNode : public DecoratedASTNode {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

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
