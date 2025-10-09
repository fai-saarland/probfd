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

class ConstructContext;

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
    virtual void dump(std::ostream& out, std::string indent = "+") const = 0;
};

class FunctionArgument {
    DecoratedASTNodePtr value;
    bool is_default;

public:
    FunctionArgument(DecoratedASTNodePtr value, bool is_default);

    DecoratedASTNode& get_value();
    const DecoratedASTNode& get_value() const;
    bool is_default_argument() const;
    void dump(std::ostream& out, const std::string& indent) const;
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
    void dump(std::ostream& out, std::string indent) const override;
};

class DecoratedFunctionCallNode : public DecoratedASTNode {
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
    void dump(std::ostream& out, std::string indent) const override;
};

class DecoratedListNode : public DecoratedASTNode {
    std::vector<DecoratedASTNodePtr> elements;

public:
    explicit DecoratedListNode(std::vector<DecoratedASTNodePtr>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;

    const std::vector<DecoratedASTNodePtr>& get_elements() const
    {
        return elements;
    }
};

class VariableNode : public DecoratedASTNode {
    friend VariableDefinition;

    VariableDefinition* definition;

public:
    explicit VariableNode(VariableDefinition& definition);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class BoolLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit BoolLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class StringLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class IntLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit IntLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class FloatLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit FloatLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class DurationLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit DurationLiteralNode(std::string value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
};

class SymbolNode : public DecoratedASTNode {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
    void dump(std::ostream& out, std::string indent) const override;
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
    void dump(std::ostream& out, std::string indent) const override;
};

class CheckBoundsNode : public DecoratedASTNode {
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
    void dump(std::ostream& out, std::string indent) const override;
};
} // namespace downward::cli::parser
#endif
