#ifndef LANGUAGE_PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H
#define LANGUAGE_PARSER_DECORATED_ABSTRACT_SYNTAX_TREE_H

#include "language/parser/decorated_expression.h"
#include "language/parser/token.h"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace language::plugins {
class Feature;
class Type;
} // namespace language::plugins

namespace language::parser {
struct Declaration;
}

namespace language::parser {

class DecoratedLetExpression : public DecoratedExpression {
    std::vector<VariableDefinition> decorated_variable_definitions;
    std::unique_ptr<DecoratedExpression> nested_value;

public:
    DecoratedLetExpression(
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

class DecoratedFunctionCallExpression : public DecoratedExpression {
    std::shared_ptr<const plugins::Feature> feature;
    std::vector<std::pair<std::string, FunctionArgument>> arguments;
    std::string unparsed_config;

public:
    DecoratedFunctionCallExpression(
        const std::shared_ptr<const plugins::Feature>& feature,
        std::vector<std::pair<std::string, FunctionArgument>>&& arguments,
        const std::string& unparsed_config);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedListExpression : public DecoratedExpression {
    std::vector<std::unique_ptr<DecoratedExpression>> elements;

public:
    explicit DecoratedListExpression(
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

class DecoratedUnaryExpression : public DecoratedExpression {
    Token token;
    std::unique_ptr<DecoratedExpression> operand;

public:
    explicit DecoratedUnaryExpression(
        Token token,
        std::unique_ptr<DecoratedExpression> operand);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedIdentifierExpression : public DecoratedExpression {
    friend Declaration;

    Declaration* declaration;

public:
    explicit DecoratedIdentifierExpression(Declaration& definition);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedBoolLiteralExpression : public DecoratedExpression {
    bool value;

public:
    explicit DecoratedBoolLiteralExpression(bool value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedStringLiteralExpression : public DecoratedExpression {
    std::string value;

public:
    explicit DecoratedStringLiteralExpression(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedIntLiteralExpression : public DecoratedExpression {
    std::string value;

public:
    explicit DecoratedIntLiteralExpression(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedFloatLiteralExpression : public DecoratedExpression {
    std::string value;

public:
    explicit DecoratedFloatLiteralExpression(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedSymbolExpression : public DecoratedExpression {
    std::string value;

public:
    explicit DecoratedSymbolExpression(const std::string& value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedConvertExpression : public DecoratedExpression {
    std::unique_ptr<DecoratedExpression> value;
    const plugins::Type& from_type;
    const plugins::Type& to_type;

public:
    DecoratedConvertExpression(
        std::unique_ptr<DecoratedExpression> value,
        const plugins::Type& from_type,
        const plugins::Type& to_type);

    void remove_variable_usages() override;

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

class DecoratedCheckBoundsExpression : public DecoratedExpression {
    std::unique_ptr<DecoratedExpression> value;
    std::unique_ptr<DecoratedExpression> min_value;
    std::unique_ptr<DecoratedExpression> max_value;

public:
    DecoratedCheckBoundsExpression(
        std::unique_ptr<DecoratedExpression> value,
        std::unique_ptr<DecoratedExpression> min_value,
        std::unique_ptr<DecoratedExpression> max_value);

    std::any construct(ConstructContext& context) const override;

    void print(std::ostream& out, std::size_t indent, bool print_default_args)
        const override;
};

} // namespace language::parser
#endif
