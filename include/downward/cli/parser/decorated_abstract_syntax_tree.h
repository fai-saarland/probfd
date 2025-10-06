#ifndef PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H

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

// TODO: if we can get rid of lazy values, this class could be moved to the cc
// file.
class ConstructContext : public utils::Context {
    std::unordered_map<std::string, std::any> variables;

public:
    void set_variable(const std::string& name, const std::any& value);
    void remove_variable(const std::string& name);
    bool has_variable(const std::string& name) const;
    std::any get_variable(const std::string& name) const;
};

class DecoratedASTNode;
using DecoratedASTNodePtr = std::unique_ptr<DecoratedASTNode>;

class VariableNode;

struct VariableDefinition {
    std::string variable_name;
    DecoratedASTNodePtr variable_expression;
    std::vector<VariableNode*> usages;

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
    virtual void dump(std::string indent = "+") const = 0;

    virtual DecoratedASTNode* clone() const = 0;

    std::unique_ptr<DecoratedASTNode> clone_unique() const
    {
        return std::unique_ptr<DecoratedASTNode>(clone());
    }

    std::shared_ptr<DecoratedASTNode> clone_shared() const
    {
        return std::shared_ptr<DecoratedASTNode>(clone());
    }
};

template <class Derived>
class CloneableDecoratedASTNode : public DecoratedASTNode {
public:
    DecoratedASTNode* clone() const override
    {
        return new Derived(static_cast<const Derived&>(*this));
    }
};

using DecoratedASTNodePtr = std::unique_ptr<DecoratedASTNode>;

class FunctionArgument {
    DecoratedASTNodePtr value;
    bool is_default;

public:
    FunctionArgument(DecoratedASTNodePtr value, bool is_default);

    DecoratedASTNode& get_value();
    const DecoratedASTNode& get_value() const;
    bool is_default_argument() const;
    void dump(const std::string& indent) const;

    FunctionArgument(const FunctionArgument& other);
    FunctionArgument(FunctionArgument&& other) = default;
};

class DecoratedLetNode : public CloneableDecoratedASTNode<DecoratedLetNode> {
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
    void dump(std::string indent) const override;

    DecoratedLetNode(const DecoratedLetNode& other);
};

class DecoratedFunctionCallNode
    : public CloneableDecoratedASTNode<DecoratedFunctionCallNode> {
    std::shared_ptr<const plugins::Feature> feature;
    std::vector<std::pair<std::string, FunctionArgument>> arguments;
    std::string unparsed_config;

public:
    DecoratedFunctionCallNode(
        const std::shared_ptr<const plugins::Feature>& feature,
        std::vector<std::pair<std::string, FunctionArgument>>&& arguments,
        const std::string& unparsed_config);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    DecoratedFunctionCallNode(const DecoratedFunctionCallNode& other);
};

class DecoratedListNode : public CloneableDecoratedASTNode<DecoratedListNode> {
    std::vector<DecoratedASTNodePtr> elements;

public:
    explicit DecoratedListNode(std::vector<DecoratedASTNodePtr>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    DecoratedListNode(const DecoratedListNode& other);

    const std::vector<DecoratedASTNodePtr>& get_elements() const
    {
        return elements;
    }
};

class VariableNode : public CloneableDecoratedASTNode<VariableNode> {
    friend VariableDefinition;

    VariableDefinition* definition;

public:
    explicit VariableNode(VariableDefinition& definition);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;
};

class BoolLiteralNode : public CloneableDecoratedASTNode<BoolLiteralNode> {
    std::string value;

public:
    explicit BoolLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    BoolLiteralNode(const BoolLiteralNode& other);
};

class StringLiteralNode : public CloneableDecoratedASTNode<StringLiteralNode> {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    StringLiteralNode(const StringLiteralNode& other);
};

class IntLiteralNode : public CloneableDecoratedASTNode<IntLiteralNode> {
    std::string value;

public:
    explicit IntLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    IntLiteralNode(const IntLiteralNode& other);
};

class FloatLiteralNode : public CloneableDecoratedASTNode<FloatLiteralNode> {
    std::string value;

public:
    explicit FloatLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    FloatLiteralNode(const FloatLiteralNode& other);
};

class SymbolNode : public CloneableDecoratedASTNode<SymbolNode> {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    SymbolNode(const SymbolNode& other);
};

class ConvertNode : public CloneableDecoratedASTNode<ConvertNode> {
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
    void dump(std::string indent) const override;

    ConvertNode(const ConvertNode& other);
};

class CheckBoundsNode : public CloneableDecoratedASTNode<CheckBoundsNode> {
    DecoratedASTNodePtr value;
    DecoratedASTNodePtr min_value;
    DecoratedASTNodePtr max_value;

public:
    CheckBoundsNode(
        DecoratedASTNodePtr value,
        DecoratedASTNodePtr min_value,
        DecoratedASTNodePtr max_value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::string indent) const override;

    CheckBoundsNode(const CheckBoundsNode& other);
};
} // namespace downward::cli::parser
#endif
