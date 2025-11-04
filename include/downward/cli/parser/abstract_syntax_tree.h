#ifndef PARSER_ABSTRACT_SYNTAX_TREE_H
#define PARSER_ABSTRACT_SYNTAX_TREE_H

#include "downward/cli/parser/decorated_abstract_syntax_tree.h"
#include "downward/cli/parser/token_stream.h"
#include "downward/cli/plugins/raw_registry.h"

#include <cassert>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace downward::cli::plugins {
struct ArgumentInfo;
class Type;
} // namespace downward::cli::plugins

namespace downward::cli::parser {
class VariableEnvironment;

struct TypedDecoratedAstNodePtr {
    DecoratedASTNodePtr ast_node;
    const plugins::Type* type;
};

class TypeNode {
public:
    virtual ~TypeNode() = default;

    virtual const plugins::Type&
    get_type(plugins::TypeRegistry& type_registry) const = 0;

    virtual void dump(std::string indent) const = 0;
};

class TypeLiteralNode : public TypeNode {
    Token value;

public:
    explicit TypeLiteralNode(const Token& value);

    const plugins::Type&
    get_type(plugins::TypeRegistry& type_registry) const override;

    void dump(std::string indent) const override;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;

    DecoratedASTNodePtr decorate(const plugins::Registry& registry) const;
    virtual TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const = 0;
    virtual void dump(std::string indent = "+") const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

class LetNode : public ASTNode {
    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions;
    ASTNodePtr nested_value;

public:
    LetNode(
        std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions,
        ASTNodePtr nested_value);

    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};

struct TypedParameter {
    std::string parameter_name;
    std::unique_ptr<TypeNode> type_node;

    TypedParameter(
        std::string parameter_name,
        std::unique_ptr<TypeNode> type_node);
};

class LambdaNode : public ASTNode {
    std::vector<TypedParameter> parameters;
    ASTNodePtr nested_value;

public:
    LambdaNode(std::vector<TypedParameter> parameters, ASTNodePtr nested_value);

    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};

class FunctionCallNode : public ASTNode {
    ASTNodePtr callee;
    std::vector<ASTNodePtr> positional_arguments;
    std::unordered_map<std::string, ASTNodePtr> keyword_arguments;
    std::string unparsed_config;

    using CollectedArguments =
        std::unordered_map<std::string, FunctionArgument>;

    static bool collect_argument(
        const ASTNode& arg,
        const plugins::ArgumentInfo& arg_info,
        utils::Context& context,
        VariableEnvironment& env,
        CollectedArguments& arguments,
        bool is_default);
    void collect_keyword_arguments(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        utils::Context& context,
        VariableEnvironment& env,
        CollectedArguments& arguments) const;
    void collect_positional_arguments(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        utils::Context& context,
        VariableEnvironment& env,
        CollectedArguments& arguments) const;
    static void collect_default_values(
        const std::vector<plugins::ArgumentInfo>& argument_infos,
        utils::Context& context,
        VariableEnvironment& env,
        CollectedArguments& arguments);

public:
    FunctionCallNode(
        ASTNodePtr callee,
        std::vector<ASTNodePtr>&& positional_arguments,
        std::unordered_map<std::string, ASTNodePtr>&& keyword_arguments,
        const std::string& unparsed_config);
    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};

class ListNode : public ASTNode {
    std::vector<ASTNodePtr> elements;

public:
    explicit ListNode(std::vector<ASTNodePtr>&& elements);
    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};

class UnaryNode : public ASTNode {
    ASTNodePtr nested_expr;
    TokenType token_type;

public:
    UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type);

    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};

class LiteralNode : public ASTNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);
    TypedDecoratedAstNodePtr
    decorate(utils::Context& context, VariableEnvironment& env) const override;
    void dump(std::string indent) const override;
};
} // namespace downward::cli::parser
#endif
