#include "language/parser/decorated_abstract_syntax_tree.h"

#include "language/parser/declaration.h"

#include "language/plugins/options.h"
#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include <algorithm>
#include <any>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <ranges>

using namespace std;

namespace {

bool is_product_within_limit(int factor1, int factor2, int limit)
{
    assert(factor1 >= 0);
    assert(factor2 >= 0);
    assert(limit >= 0);
    return factor2 == 0 || factor1 <= limit / factor2;
}

bool is_product_within_limit_unsigned(
    unsigned int factor1,
    unsigned int factor2,
    unsigned int limit)
{
    return factor2 == 0 || factor1 <= limit / factor2;
}

unsigned int safe_abs(int x)
{
    // Don't call abs() if the call would overflow.
    if (x == numeric_limits<int>::min()) {
        return static_cast<unsigned int>(-(x + 1)) + 1u;
    }
    return abs(x);
}

bool is_product_within_limits(
    int factor1,
    int factor2,
    int lower_limit,
    int upper_limit)
{
    assert(lower_limit < 0);
    assert(upper_limit >= 0);

    if (factor1 >= 0 && factor2 >= 0) {
        return is_product_within_limit(factor1, factor2, upper_limit);
    }

    if (factor1 < 0 && factor2 < 0) {
        return is_product_within_limit_unsigned(
            safe_abs(factor1),
            safe_abs(factor2),
            upper_limit);
    }

    return is_product_within_limit_unsigned(
        safe_abs(factor1),
        safe_abs(factor2),
        safe_abs(lower_limit));
}

} // namespace

namespace language::parser {

class ConstructContext : public Context {
    std::vector<std::any> variables;

public:
    void push_variable(std::any value);
    void pop_variable();

    std::any get_variable(std::size_t index) const;

    std::size_t get_stack_offset() const;
};

void ConstructContext::push_variable(std::any value)
{
    variables.emplace_back(std::move(value));
}

void ConstructContext::pop_variable()
{
    variables.pop_back();
}

std::any ConstructContext::get_variable(std::size_t index) const
{
    return variables[index];
}

std::size_t ConstructContext::get_stack_offset() const
{
    return variables.size();
}

std::vector<VariableDefinition> DecoratedExpression::prune_unused_definitions()
{
    std::vector<VariableDefinition> defs;
    prune_unused_definitions(defs);
    return defs;
}

std::any DecoratedExpression::construct() const
{
    ConstructContext context;
    TraceBlock _(context, "Constructing parsed object");
    return construct(context);
}

FunctionArgument::FunctionArgument(
    std::unique_ptr<DecoratedExpression> value,
    bool is_default)
    : value(std::move(value))
    , is_default(is_default)
{
}

DecoratedExpression& FunctionArgument::get_value()
{
    return *value;
}

const DecoratedExpression& FunctionArgument::get_value() const
{
    return *value;
}

bool FunctionArgument::is_default_argument() const
{
    return is_default;
}

DecoratedLetExpression::DecoratedLetExpression(
    std::vector<VariableDefinition> decorated_variable_definitions,
    std::unique_ptr<DecoratedExpression> nested_value)
    : decorated_variable_definitions(std::move(decorated_variable_definitions))
    , nested_value(std::move(nested_value))
{
}

void DecoratedLetExpression::prune_unused_definitions(
    std::vector<VariableDefinition>& defs)
{
    nested_value->prune_unused_definitions(defs);

    for (auto& declaration :
         std::views::reverse(decorated_variable_definitions)) {
        if (declaration.usages.empty()) {
            declaration.variable_expression->remove_variable_usages();
        }
    }

    auto r = std::ranges::stable_partition(
        decorated_variable_definitions,
        [](const auto& declaration) { return !declaration.usages.empty(); });

    defs.append_range(r | std::views::as_rvalue);

    decorated_variable_definitions.erase(r.begin(), r.end());
}

void DecoratedLetExpression::remove_variable_usages()
{
    for (const auto& declaration : decorated_variable_definitions) {
        declaration.variable_expression->remove_variable_usages();
    }
}

std::any DecoratedLetExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing let-expression");
    for (const auto& declaration : decorated_variable_definitions) {
        TraceBlock _(
            context,
            "Constructing variable '{}'",
            declaration.identifier);
        declaration.stack_index = context.get_stack_offset();
        context.push_variable(
            declaration.variable_expression->construct(context));
    }

    std::any result;
    {
        TraceBlock _(context, "Constructing nested value");
        result = nested_value->construct(context);
    }

    for (std::size_t i = 0; i != decorated_variable_definitions.size(); ++i) {
        context.pop_variable();
    }

    return result;
}

void DecoratedLetExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::println(out, "{:>{}}", "let", indent + 3);

    if (!decorated_variable_definitions.empty()) {
        {
            const auto& declaration = decorated_variable_definitions.front();
            declaration.variable_expression->print(
                out,
                indent + 4,
                print_default_args);
            std::print(out, " as {}", declaration.identifier);
        }

        for (const auto& declaration :
             decorated_variable_definitions | std::views::drop(1)) {
            std::println(out, ",");
            declaration.variable_expression->print(
                out,
                indent + 4,
                print_default_args);
            std::print(out, " as {}", declaration.identifier);
        }
    }

    std::println(out);
    std::println(out, "{:>{}}", "in", indent + 2);

    nested_value->print(out, indent + 4, print_default_args);
}

DecoratedFunctionCallExpression::DecoratedFunctionCallExpression(
    const shared_ptr<const plugins::Feature>& feature,
    vector<std::pair<std::string, FunctionArgument>>&& arguments,
    string unparsed_config)
    : feature(feature)
    , arguments(std::move(arguments))
    , unparsed_config(std::move(unparsed_config))
{
}

void DecoratedFunctionCallExpression::remove_variable_usages()
{
    for (auto& arg : arguments | views::values) {
        arg.get_value().remove_variable_usages();
    }
}

std::any
DecoratedFunctionCallExpression::construct(ConstructContext& context) const
{
    TraceBlock _(
        context,
        "Constructing feature '{}': {}",
        feature->get_key(),
        unparsed_config);

    plugins::Options opts;
    opts.set_unparsed_config(unparsed_config);
    for (const auto& [key, arg] : arguments) {
        TraceBlock _(context, "Constructing argument '{}'", key);
        opts.set(key, arg.get_value().construct(context));
    }
    return feature->construct(opts, context);
}

void DecoratedFunctionCallExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(
        out,
        "{:>{}}(",
        feature->get_key(),
        indent + feature->get_key().size());

    auto filter =
        print_default_args
            ? static_cast<std::function<bool(
                  const std::pair<std::string, FunctionArgument>&)>>(
                  [](const auto&) { return true; })
            : [](const auto& arg) { return !arg.second.is_default_argument(); };

    if (auto args = arguments | std::views::filter(filter); !args.empty()) {
        {
            const auto& [key, arg] = args.front();
            std::print(out, "{}=", key);
            arg.get_value().print(out, 0, print_default_args);
        }

        for (const auto& [key, arg] : args | std::views::drop(1)) {
            std::print(out, ", {}=", key);
            arg.get_value().print(out, 0, print_default_args);
        }
    }

    std::print(out, ")");
}

DecoratedListExpression::DecoratedListExpression(
    vector<std::unique_ptr<DecoratedExpression>>&& elements)
    : elements(std::move(elements))
{
}

void DecoratedListExpression::remove_variable_usages()
{
    for (const auto& el : elements) {
        el->remove_variable_usages();
    }
}

std::any DecoratedListExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing list");
    vector<std::any> result;
    int i = 0;
    for (const std::unique_ptr<DecoratedExpression>& element : elements) {
        TraceBlock _(context, "Constructing element {}", i);
        result.push_back(element->construct(context));
        ++i;
    }
    return result;
}

void DecoratedListExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{:>{}}", "[", indent + 1);

    if (!elements.empty()) {
        {
            const auto& el = elements.front();
            el->print(out, 0, print_default_args);
        }

        for (const auto& el : elements | std::views::drop(1)) {
            std::print(out, ", ");
            el->print(out, 0, print_default_args);
        }
    }

    std::print(out, "]");
}

DecoratedUnaryExpression::DecoratedUnaryExpression(
    Token token,
    std::unique_ptr<DecoratedExpression> operand)
    : token(std::move(token))
    , operand(std::move(operand))
{
}

void DecoratedUnaryExpression::remove_variable_usages()
{
    operand->remove_variable_usages();
}

std::any DecoratedUnaryExpression::construct(ConstructContext& context) const
{
    std::any operand_value = operand->construct(context);

    switch (token.type) {
    case TokenType::PLUS: return std::any_cast<int>(operand_value);
    case TokenType::MINUS: return -std::any_cast<int>(operand_value);
    default: context.error("Unknown unary expression operand: {}", token);
    }
}

void DecoratedUnaryExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    for (std::size_t i = 0; i != indent; ++i) {
        out.put(' ');
    }
    std::print(out, "{}", token.content);
    operand->print(out, 0, print_default_args);
}

DecoratedLocalIdentifierExpression::DecoratedLocalIdentifierExpression(
    VariableDefinition& declaration)
    : declaration(&declaration)
{
}

void DecoratedLocalIdentifierExpression::remove_variable_usages()
{
    const auto it = std::ranges::find(declaration->usages, this);
    assert(it != declaration->usages.end());
    declaration->usages.erase(it);
}

std::any
DecoratedLocalIdentifierExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Looking up variable '{}'", declaration->identifier);
    return context.get_variable(declaration->stack_index);
}

void DecoratedLocalIdentifierExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(
        out,
        "{:>{}}",
        declaration->identifier,
        indent + declaration->identifier.size());
}

DecoratedBoolLiteralExpression::DecoratedBoolLiteralExpression(bool value)
    : value(value)
{
}

std::any
DecoratedBoolLiteralExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing bool value from '{}'", value);
    return value;
}

void DecoratedBoolLiteralExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

DecoratedStringLiteralExpression::DecoratedStringLiteralExpression(string value)
    : value(std::move(value))
{
}

std::any
DecoratedStringLiteralExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing string value from '{}'", value);
    if (!(value.starts_with('"') && value.ends_with('"'))) {
        context.error(
            "String literal value is not enclosed in quotation marks"
            " (this should have been caught before constructing this "
            "Expression).");
    }
    /*
      We are not doing any further syntax checking. Escaped symbols other than
      \n will just ignore the escaping \ (e.g., \t is treated as t, not as a
      tab). Strings ending in \ will not produce an error but should be excluded
      by the previous steps.
    */
    string result;
    result.reserve(value.length() - 2);
    bool escaped = false;
    for (const char c : value.substr(1, value.size() - 2)) {
        if (escaped) {
            escaped = false;
            if (c == 'n') {
                result += '\n';
            } else {
                result += c;
            }
        } else if (c == '\\') {
            escaped = true;
        } else {
            result += c;
        }
    }
    return result;
}

void DecoratedStringLiteralExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(out, "{:>{}}", value, indent + value.size());
}

DecoratedIntLiteralExpression::DecoratedIntLiteralExpression(string value)
    : value(std::move(value))
{
}

std::any
DecoratedIntLiteralExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing int value from '{}'", value);
    if (value.empty()) {
        context.error(
            "Empty value in int constant '{}'"
            " (this should have been caught before constructing this "
            "Expression).",
            value);
    }

    if (value == "infinity") {
        return numeric_limits<int>::max();
    }

    char suffix = value.back();
    string prefix = value;
    int factor = 1;
    if (isalpha(suffix)) {
        suffix = static_cast<char>(tolower(suffix));
        if (suffix == 'k' || suffix == 'K') {
            factor = 1000;
        } else if (suffix == 'm' || suffix == 'M') {
            factor = 1'000'000;
        } else if (suffix == 'g' || suffix == 'G') {
            factor = 1'000'000'000;
        } else {
            context.error(
                "Invalid suffix in int constant '{}'"
                " (this should have been caught before constructing this "
                "Expression).",
                value);
        }
        prefix.pop_back();
    }

    istringstream stream(prefix);
    int x;
    stream >> noskipws >> x;
    if (stream.fail() || !stream.eof()) {
        context.error(
            "Could not parse int constant '{}'"
            " (this should have been caught before constructing this "
            "Expression).",
            value);
    }

    constexpr int min_int = numeric_limits<int>::min();
    // Reserve highest value for "infinity".
    constexpr int max_int = numeric_limits<int>::max() - 1;
    if (!is_product_within_limits(x, factor, min_int, max_int)) {
        context.error(
            "Absolute value of integer constant too large: '{}'",
            value);
    }
    return x * factor;
}

void DecoratedIntLiteralExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

DecoratedFloatLiteralExpression::DecoratedFloatLiteralExpression(string value)
    : value(std::move(value))
{
}

std::any
DecoratedFloatLiteralExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing float value from '{}'", value);
    if (value == "infinity") {
        return numeric_limits<double>::infinity();
    }

    istringstream stream(value);
    double x;
    stream >> noskipws >> x;
    if (stream.fail() || !stream.eof()) {
        context.error(
            "Could not parse double constant '{}"
            " (this should have been caught before constructing this "
            "Expression).",
            value);
    }
    return x;
}

void DecoratedFloatLiteralExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

DecoratedSymbolExpression::DecoratedSymbolExpression(string value)
    : value(std::move(value))
{
}

std::any DecoratedSymbolExpression::construct(ConstructContext&) const
{
    return std::any(value);
}

void DecoratedSymbolExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

DecoratedConvertExpression::DecoratedConvertExpression(
    std::unique_ptr<DecoratedExpression> value,
    const plugins::Type& from_type,
    const plugins::Type& to_type)
    : value(std::move(value))
    , from_type(from_type)
    , to_type(to_type)
{
}

void DecoratedConvertExpression::remove_variable_usages()
{
    value->remove_variable_usages();
}

std::any DecoratedConvertExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing value that requires conversion");
    std::any constructed_value;
    {
        TraceBlock _(
            context,
            "Constructing value of type '{}'",
            from_type.name());
        constructed_value = value->construct(context);
    }
    std::any converted_value;
    {
        TraceBlock _(
            context,
            "Converting constructed value from '{}' to '{}'",
            from_type.name(),
            to_type.name());

        converted_value =
            plugins::convert(constructed_value, from_type, to_type, context);
    }
    return converted_value;
}

void DecoratedConvertExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

} // namespace language::parser