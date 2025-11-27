#ifndef PARSER_ABSTRACT_SYNTAX_TREE_H
#define PARSER_ABSTRACT_SYNTAX_TREE_H

#include "downward/cli/parser/decorated_abstract_syntax_tree.h"
#include "downward/cli/parser/token_stream.h"
#include "downward/cli/parser/token.h"

#include "downward/cli/plugins/registry.h"

#include "downward/utils/strings.h"

#include <memory>
#include <string>
#include <unordered_map>
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

struct QualifiedName {
    std::vector<std::string> qualification_prefix;
    std::string name;
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

    DecoratedASTNodePtr
    static_analysis(const plugins::Registry& registry) const;

    virtual TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const = 0;

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
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

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
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

class DirectFunctionCallNode : public ASTNode {
    QualifiedName callee;
    std::vector<ASTNodePtr> positional_arguments;
    std::unordered_map<std::string, ASTNodePtr> keyword_arguments;
    std::string unparsed_config;

public:
    DirectFunctionCallNode(
        QualifiedName callee,
        std::vector<ASTNodePtr>&& positional_arguments,
        std::unordered_map<std::string, ASTNodePtr>&& keyword_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

class IndirectFunctionCallNode : public ASTNode {
    ASTNodePtr callee;
    std::vector<ASTNodePtr> positional_arguments;
    std::string unparsed_config;

public:
    IndirectFunctionCallNode(
        ASTNodePtr callee,
        std::vector<ASTNodePtr>&& positional_arguments,
        const std::string& unparsed_config);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

class ListNode : public ASTNode {
    std::vector<ASTNodePtr> elements;

public:
    explicit ListNode(std::vector<ASTNodePtr>&& elements);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

class UnaryNode : public ASTNode {
    ASTNodePtr nested_expr;
    TokenType token_type;

public:
    UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

class IdentifierNode : public ASTNode {
    QualifiedName qualified_name;

public:
    explicit IdentifierNode(QualifiedName qualified_name);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;

    const QualifiedName& get_name() const;
};

class LiteralNode : public ASTNode {
    Token value;

public:
    explicit LiteralNode(const Token& value);

    TypedDecoratedAstNodePtr
    static_analysis(utils::Context& context, VariableEnvironment& env)
        const override;

    void dump(std::string indent) const override;
};

} // namespace downward::cli::parser

// Formatter specialization for QualifiedName
template <>
struct std::formatter<downward::cli::parser::QualifiedName> {
    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx);

    template <typename FormatContext>
    typename FormatContext::iterator
    format(const downward::cli::parser::QualifiedName& name, FormatContext& ctx)
        const;
};

template <typename ParseContext>
constexpr typename ParseContext::iterator
std::formatter<downward::cli::parser::QualifiedName, char>::parse(
    ParseContext& ctx)
{
    return ctx.begin();
}

template <typename FormatContext>
typename FormatContext::iterator
std::formatter<downward::cli::parser::QualifiedName, char>::format(
    const downward::cli::parser::QualifiedName& name,
    FormatContext& ctx) const
{
    if (name.qualification_prefix.empty()) {
        return std::format_to(ctx.out(), "{}", name.name);
    }

    return std::format_to(
        ctx.out(),
        "{}.{}",
        downward::utils::join_view(name.qualification_prefix, "."),
        name.name);
}

#endif
