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

class LetNode : public Expression {
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>>
        variable_definitions;
    std::unique_ptr<Expression> nested_value;

public:
    LetNode(
        std::vector<std::pair<std::string, std::unique_ptr<Expression>>> variable_definitions,
        std::unique_ptr<Expression> nested_value);

    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class FunctionCallNode : public Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> positional_arguments;
    std::unordered_map<std::string, std::unique_ptr<Expression>> keyword_arguments;
    std::string unparsed_config;

    using CollectedArguments =
        std::unordered_map<std::string, FunctionArgument>;

    static bool collect_argument(
        Expression& arg,
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
    FunctionCallNode(
        const std::string& name,
        std::vector<std::unique_ptr<Expression>>&& positional_arguments,
        std::unordered_map<std::string, std::unique_ptr<Expression>>&& keyword_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class ListNode : public Expression {
    std::vector<std::unique_ptr<Expression>> elements;

public:
    explicit ListNode(std::vector<std::unique_ptr<Expression>>&& elements);

    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};

class LiteralNode : public Expression {
    Token value;

public:
    explicit LiteralNode(const Token& value);

    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;

    void dump(std::string indent) const override;
};
} // namespace language::parser
#endif
