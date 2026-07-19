#ifndef LANGUAGE_PARSER_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_PARSER_ABSTRACT_SYNTAX_TREE_H

#include "language/parser/decorated_abstract_syntax_tree.h"
#include "language/parser/expression.h"
#include "language/parser/token.h"

#include "language/plugins/raw_registry.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace language::plugins {
struct ArgumentInfo;
} // namespace language::plugins

namespace language::parser {

class LetExpression : public Expression {
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>>
        variable_definitions;
    std::unique_ptr<Expression> nested_value;

public:
    LetExpression(
        std::vector<std::pair<std::string, std::unique_ptr<Expression>>>
            variable_definitions,
        std::unique_ptr<Expression> nested_value);

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class FunctionCallExpression : public Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> positional_arguments;
    std::unordered_map<std::string, std::unique_ptr<Expression>>
        keyword_arguments;
    std::string unparsed_config;

    using CollectedArguments =
        std::unordered_map<std::string, FunctionArgument>;

    static bool collect_argument(
        const Expression& arg,
        const plugins::ArgumentInfo& arg_info,
        DecorateContext& context,
        CollectedArguments& arguments,
        bool is_default);

    void collect_keyword_arguments(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        DecorateContext& context,
        CollectedArguments& arguments) const;

    void collect_positional_arguments(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        DecorateContext& context,
        CollectedArguments& arguments) const;

    static void collect_default_values(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        DecorateContext& context,
        CollectedArguments& arguments);

public:
    FunctionCallExpression(
        std::unique_ptr<Expression> callee,
        std::vector<std::unique_ptr<Expression>>&& positional_arguments,
        std::unordered_map<std::string, std::unique_ptr<Expression>>&&
            keyword_arguments,
        std::string unparsed_config);

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class ListExpression : public Expression {
    std::vector<std::unique_ptr<Expression>> elements;

public:
    explicit ListExpression(
        std::vector<std::unique_ptr<Expression>>&& elements);

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class IdentifierExpression : public Expression {
    Token identifier;

public:
    explicit IdentifierExpression(Token identifier);

    const Token& get_identifier() const;

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class LiteralExpression : public Expression {
    Token value;

public:
    explicit LiteralExpression(Token value);

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class PrefixExpression : public Expression {
    Token expr_operator;
    std::unique_ptr<Expression> operand;

public:
    explicit PrefixExpression(
        Token expr_operator,
        std::unique_ptr<Expression> operand);

    TypedDecoratedExpressionPtr
    decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

} // namespace language::parser
#endif
