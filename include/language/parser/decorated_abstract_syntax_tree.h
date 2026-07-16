#ifndef LANGUAGE_PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "language/plugins/plugin.h"

#include "language/parser/decorated_expression.h"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace language::parser {

class DecoratedLetNode : public DecoratedExpression {
    std::vector<VariableDefinition> decorated_variable_definitions;
    std::unique_ptr<DecoratedExpression> nested_value;

public:
    DecoratedLetNode(
        std::vector<VariableDefinition> decorated_variable_definitions,
        std::unique_ptr<DecoratedExpression> nested_value);

    void
    prune_unused_definitions(std::vector<VariableDefinition>& defs) override;

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class FunctionArgument {
    std::unique_ptr<DecoratedExpression> value;
    bool is_default;

public:
    FunctionArgument(
        std::unique_ptr<DecoratedExpression> value,
        bool is_default);

    DecoratedExpression& get_value();

    const DecoratedExpression& get_value() const;

    bool is_default_argument() const;
};

class DecoratedFunctionCallNode : public DecoratedExpression {
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
};

class DecoratedListNode : public DecoratedExpression {
    std::vector<std::unique_ptr<DecoratedExpression>> elements;

public:
    explicit DecoratedListNode(
        std::vector<std::unique_ptr<DecoratedExpression>>&& elements);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;

    const std::vector<std::unique_ptr<DecoratedExpression>>&
    get_elements() const
    {
        return elements;
    }
};

class VariableNode : public DecoratedExpression {
    friend VariableDefinition;

    VariableDefinition* definition;

public:
    explicit VariableNode(VariableDefinition& definition);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class BoolLiteralNode : public DecoratedExpression {
    bool value;

public:
    explicit BoolLiteralNode(bool value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class StringLiteralNode : public DecoratedExpression {
    std::string value;

public:
    explicit StringLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class IntLiteralNode : public DecoratedExpression {
    std::string value;

public:
    explicit IntLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class FloatLiteralNode : public DecoratedExpression {
    std::string value;

public:
    explicit FloatLiteralNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class SymbolNode : public DecoratedExpression {
    std::string value;

public:
    explicit SymbolNode(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class ConvertNode : public DecoratedExpression {
    std::unique_ptr<DecoratedExpression> value;
    const plugins::Type& from_type;
    const plugins::Type& to_type;

public:
    ConvertNode(
        std::unique_ptr<DecoratedExpression> value,
        const plugins::Type& from_type,
        const plugins::Type& to_type);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class CheckBoundsNode : public DecoratedExpression {
    std::unique_ptr<DecoratedExpression> value;
    std::unique_ptr<DecoratedExpression> min_value;
    std::unique_ptr<DecoratedExpression> max_value;

public:
    CheckBoundsNode(
        std::unique_ptr<DecoratedExpression> value,
        std::unique_ptr<DecoratedExpression> min_value,
        std::unique_ptr<DecoratedExpression> max_value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
