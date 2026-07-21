#include "language/parser/decorated_abstract_syntax_tree.h"

#include "language/parser/declaration.h"

#include "language/plugins/options.h"
#include "language/plugins/plugin.h"
#include "language/plugins/types.h"

#include <any>
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
    std::unordered_map<std::string, std::any> variables;

public:
    void set_variable(const std::string& name, const std::any& value);
    void remove_variable(const std::string& name);
    bool has_variable(const std::string& name) const;
    std::any get_variable(const std::string& name) const;
};

void ConstructContext::set_variable(const string& name, const std::any& value)
{
    variables[name] = value;
}

void ConstructContext::remove_variable(const string& name)
{
    variables.erase(name);
}

bool ConstructContext::has_variable(const string& name) const
{
    return variables.contains(name);
}

std::any ConstructContext::get_variable(const string& name) const
{
    std::any variable = variables.at(name);
    return variable;
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
    : value(move(value))
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
    : decorated_variable_definitions(move(decorated_variable_definitions))
    , nested_value(move(nested_value))
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

    auto [beg, end] = std::ranges::stable_partition(
        decorated_variable_definitions,
        [](const auto& declaration) { return !declaration.usages.empty(); });

    defs.insert(
        defs.end(),
        std::make_move_iterator(beg),
        std::make_move_iterator(end));

    decorated_variable_definitions.erase(beg, end);
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
        std::any variable_value =
            declaration.variable_expression->construct(context);
        context.set_variable(declaration.identifier, variable_value);
    }

    std::any result;
    {
        TraceBlock _(context, "Constructing nested value");
        result = nested_value->construct(context);
    }

    for (const auto& declaration : decorated_variable_definitions) {
        context.remove_variable(declaration.identifier);
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
    const string& unparsed_config)
    : feature(feature)
    , arguments(move(arguments))
    , unparsed_config(unparsed_config)
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
    : elements(move(elements))
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

DecoratedIdentifierExpression::DecoratedIdentifierExpression(
    Declaration& declaration)
    : declaration(&declaration)
{
}

void DecoratedIdentifierExpression::remove_variable_usages()
{
    const auto it = std::ranges::find(declaration->usages, this);
    assert(it != declaration->usages.end());
    declaration->usages.erase(it);
}

std::any
DecoratedIdentifierExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Looking up variable '{}'", declaration->identifier);

    if (!context.has_variable(declaration->identifier)) {
        context.error("Variable '{}' is not defined.", declaration->identifier);
    }

    return context.get_variable(declaration->identifier);
}

void DecoratedIdentifierExpression::print(
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

DecoratedStringLiteralExpression::DecoratedStringLiteralExpression(
    const string& value)
    : value(value)
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

DecoratedIntLiteralExpression::DecoratedIntLiteralExpression(
    const string& value)
    : value(value)
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
        if (suffix == 'k') {
            factor = 1000;
        } else if (suffix == 'm') {
            factor = 1'000'000;
        } else if (suffix == 'g') {
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

    int min_int = numeric_limits<int>::min();
    // Reserve highest value for "infinity".
    int max_int = numeric_limits<int>::max() - 1;
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

DecoratedFloatLiteralExpression::DecoratedFloatLiteralExpression(
    const string& value)
    : value(value)
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

DecoratedSymbolExpression::DecoratedSymbolExpression(const string& value)
    : value(value)
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
    : value(move(value))
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

DecoratedCheckBoundsExpression::DecoratedCheckBoundsExpression(
    std::unique_ptr<DecoratedExpression> value,
    std::unique_ptr<DecoratedExpression> min_value,
    std::unique_ptr<DecoratedExpression> max_value)
    : value(move(value))
    , min_value(move(min_value))
    , max_value(move(max_value))
{
}

template <typename T>
static bool
satisfies_bounds(const std::any& v_, const std::any& min_, const std::any& max_)
{
    T v = std::any_cast<T>(v_);
    T min = std::any_cast<T>(min_);
    T max = std::any_cast<T>(max_);
    return min <= v && v <= max;
}

std::any
DecoratedCheckBoundsExpression::construct(ConstructContext& context) const
{
    TraceBlock _(context, "Constructing value with bounds");
    std::any v;
    {
        TraceBlock _(context, "Constructing value");
        v = value->construct(context);
    }
    std::any min;
    {
        TraceBlock _(context, "Constructing lower bound");
        min = min_value->construct(context);
    }
    std::any max;
    {
        TraceBlock _(context, "Constructing upper bound");
        max = max_value->construct(context);
    }
    {
        TraceBlock _(context, "Checking bounds");
        const type_info& type = v.type();
        if (min.type() != type || max.type() != type) {
            context.error(
                "Types of bounds ({}, {}) do not match type of value ({})"
                " (this should have been caught before constructing this "
                "Expression).",
                min.type().name(),
                max.type().name(),
                type.name());
        }

        bool bounds_satisfied = true;
        if (type == typeid(int)) {
            bounds_satisfied = satisfies_bounds<int>(v, min, max);
        } else if (type == typeid(double)) {
            bounds_satisfied = satisfies_bounds<double>(v, min, max);
        } else {
            context.error(
                "Bounds are only supported for arguments of type int or "
                "double.");
        }
        if (!bounds_satisfied) {
            context.error("Value is not in bounds.");
        }
    }
    return v;
}

void DecoratedCheckBoundsExpression::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

} // namespace language::parser