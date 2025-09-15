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

namespace downward::plugins {
struct ArgumentInfo;
class Type;
} // namespace downward::plugins

namespace downward::cli::parser {
class DecorateContext;

struct TypedDecoratedAstNodePtr {
    DecoratedASTNodePtr ast_node;
    const plugins::Type* type;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;

    DecoratedASTNodePtr
    decorate(const plugins::RawRegistry& raw_registry) const;
    virtual TypedDecoratedAstNodePtr
    decorate(DecorateContext& context) const = 0;
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

    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;
    void dump(std::string indent) const override;
};

class FunctionCallNode : public ASTNode {
    std::string name;
    std::vector<ASTNodePtr> positional_arguments;
    std::unordered_map<std::string, ASTNodePtr> keyword_arguments;
    std::string unparsed_config;

    using CollectedArguments =
        std::unordered_map<std::string, FunctionArgument>;

    static bool collect_argument(
        ASTNode& arg,
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
        std::vector<ASTNodePtr>&& positional_arguments,
        std::unordered_map<std::string, ASTNodePtr>&& keyword_arguments,
        const std::string& unparsed_config);
    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;
    void dump(std::string indent) const override;
};

class ListNode : public ASTNode {
    std::vector<ASTNodePtr> elements;

public:
    explicit ListNode(std::vector<ASTNodePtr>&& elements);
    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;
    void dump(std::string indent) const override;
};

class LiteralNode : public ASTNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);
    TypedDecoratedAstNodePtr decorate(DecorateContext& context) const override;
    void dump(std::string indent) const override;
};
} // namespace downward::cli::parser
#endif
