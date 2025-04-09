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

    void prune_unused_definitions();
    std::any construct() const;

    virtual void prune_unused_definitions(std::vector<VariableDefinition>&) {}

    virtual void remove_variable_usages() {}

    virtual std::any construct(ConstructContext& context) const = 0;
    virtual void print(std::ostream& out, std::size_t indent, bool print_default_args) const = 0;
    virtual void dump(std::string indent = "+") const = 0;

    // TODO: This is here only for the iterated search. Once we switch to
    // builders, we won't need it any more.
    virtual std::unique_ptr<DecoratedASTNode> clone() const = 0;
    virtual std::shared_ptr<DecoratedASTNode> clone_shared() const = 0;
};

using DecoratedASTNodePtr = std::unique_ptr<DecoratedASTNode>;

class LazyValue {
    ConstructContext context;
    DecoratedASTNodePtr node;
    std::any construct_any() const;

public:
    LazyValue(const DecoratedASTNode& node, const ConstructContext& context);
    LazyValue(const LazyValue& other);

    template <typename T>
    T construct() const
    {
        std::any constructed = construct_any();
        return plugins::OptionsAnyCaster<T>::cast(constructed);
    }

    std::vector<LazyValue> construct_lazy_list();
};

class FunctionArgument {
    std::string key;
    DecoratedASTNodePtr value;
    bool is_default;

    // TODO: This is here only for the iterated search. Once we switch to
    // builders, we won't need it any more.
    bool lazy_construction;

public:
    FunctionArgument(
        const std::string& key,
        DecoratedASTNodePtr value,
        bool is_default,
        bool lazy_construction);

    std::string get_key() const;
    DecoratedASTNode& get_value();
    const DecoratedASTNode& get_value() const;
    bool is_default_argument() const;
    void dump(const std::string& indent) const;

    // TODO: This is here only for the iterated search. Once we switch to
    // builders, we won't need it any more.
    bool is_lazily_constructed() const;
    FunctionArgument(const FunctionArgument& other);
    FunctionArgument(FunctionArgument&& other) = default;
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
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    DecoratedLetNode(const DecoratedLetNode& other);
};

class DecoratedFunctionCallNode : public DecoratedASTNode {
    std::shared_ptr<const plugins::Feature> feature;
    std::vector<FunctionArgument> arguments;
    std::string unparsed_config;

public:
    DecoratedFunctionCallNode(
        const std::shared_ptr<const plugins::Feature>& feature,
        std::vector<FunctionArgument>&& arguments,
        const std::string& unparsed_config);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    DecoratedFunctionCallNode(const DecoratedFunctionCallNode& other);
};

class DecoratedListNode : public DecoratedASTNode {
    std::vector<DecoratedASTNodePtr> elements;

public:
    explicit DecoratedListNode(std::vector<DecoratedASTNodePtr>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    DecoratedListNode(const DecoratedListNode& other);

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
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
};

class BoolLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit BoolLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    BoolLiteralNode(const BoolLiteralNode& other);
};

class StringLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    StringLiteralNode(const StringLiteralNode& other);
};

class IntLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit IntLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    IntLiteralNode(const IntLiteralNode& other);
};

class FloatLiteralNode : public DecoratedASTNode {
    std::string value;

public:
    explicit FloatLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    FloatLiteralNode(const FloatLiteralNode& other);
};

class SymbolNode : public DecoratedASTNode {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    SymbolNode(const SymbolNode& other);
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
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    ConvertNode(const ConvertNode& other);
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
    void print(std::ostream& out, std::size_t indent, bool print_default_args) const override;
    void dump(std::string indent) const override;

    // TODO: once we get rid of lazy construction, this should no longer be
    // necessary.
    std::unique_ptr<DecoratedASTNode> clone() const override;
    std::shared_ptr<DecoratedASTNode> clone_shared() const override;
    CheckBoundsNode(const CheckBoundsNode& other);
};
} // namespace downward::cli::parser
#endif
