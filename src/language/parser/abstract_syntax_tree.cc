#include "language/parser/abstract_syntax_tree.h"

#include "language/parser/decorated_abstract_syntax_tree.h"
#include "language/parser/lexical_analyzer.h"
#include "language/parser/syntax_analyzer.h"
#include "language/parser/token_stream.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"
#include "language/plugins/types.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <vector>

using namespace std;

namespace language::parser {

struct VariableDefinition;

namespace {
struct TypedDefinition {
    const plugins::Type* type;
    VariableDefinition* definition;
};

class Scope {
    std::unordered_map<string, TypedDefinition> variables;

public:
    void insert(const Context& context, std::pair<string, TypedDefinition> pair)
    {
        if (!variables.insert(pair).second) {
            context.error(
                "Variable '{}' is already defined in the current scope.",
                pair.first);
        }
    }

    const TypedDefinition* get_typed_definition(const string& name) const
    {
        if (const auto it = variables.find(name); it != variables.end()) {
            return &it->second;
        }

        return nullptr;
    }
};
} // namespace

class DecorateContext : public Context {
    const plugins::Registry registry;
    std::deque<Scope> scopes;

public:
    explicit DecorateContext(const plugins::RawRegistry& raw_registry)
        : registry(raw_registry.construct_registry())
        , scopes(1)
    {
    }

    void add_variable(
        const string& name,
        const plugins::Type& type,
        VariableDefinition& definition)
    {
        scopes.back().insert(
            *this,
            {name, TypedDefinition{.type = &type, .definition = &definition}});
    }

    const TypedDefinition* get_typed_definition(const string& name) const
    {
        for (const auto& scope : scopes | std::views::reverse) {
            if (const auto td = scope.get_typed_definition(name)) { return td; }
        }

        return nullptr;
    }

    void enter_scope() { scopes.emplace_back(); }

    void leave_scope() { scopes.pop_back(); }

    const plugins::Registry& get_registry() const { return registry; }
};

template <typename T, typename K>
static vector<T> get_keys(const unordered_map<T, K>& map)
{
    vector<T> keys;
    keys.reserve(map.size());
    for (const auto& key_value : map) { keys.push_back(key_value.first); }
    return keys;
}

static std::unique_ptr<Expression>
parse_expression(const string& definition, DecorateContext&)
{
    TokenStream tokens = split_tokens(definition);
    return parse(tokens);
}

std::unique_ptr<DecoratedExpression>
Expression::decorate(const plugins::RawRegistry& raw_registry) const
{
    DecorateContext context(raw_registry);
    TraceBlock block(context, "Start semantic analysis");
    return decorate(context).expression;
}

LetExpression::LetExpression(
    std::vector<std::pair<std::string, std::unique_ptr<Expression>>>
        variable_definitions,
    std::unique_ptr<Expression> nested_value)
    : variable_definitions(std::move(variable_definitions))
    , nested_value(std::move(nested_value))
{
}

TypedDecoratedExpressionPtr
LetExpression::decorate(DecorateContext& context) const
{
    TraceBlock _(
        context,
        "Checking Let: {:n}",
        variable_definitions | views::keys);

    std::vector<VariableDefinition> decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    context.enter_scope();

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        TraceBlock _(context, "Check variable definition");
        auto [expression, type] = variable_definition->decorate(context);
        auto& definition = decorated_variable_definitions.emplace_back(
            variable_name,
            std::move(expression));

        context.add_variable(variable_name, *type, definition);
    }

    TypedDecoratedExpressionPtr decorated_nested_value;

    {
        TraceBlock _(context, "Check nested expression.");
        decorated_nested_value = nested_value->decorate(context);
    }

    context.leave_scope();

    return {
        .expression = std::make_unique<DecoratedLetExpression>(
            std::move(decorated_variable_definitions),
            std::move(decorated_nested_value.expression)),
        .type = decorated_nested_value.type};
}

void LetExpression::dump(string indent) const
{
    cout << indent << "LET:";

    indent = "| " + indent;

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        cout << variable_name << " = " << endl;
        variable_definition->dump(indent);
    }

    cout << indent << "IN:" << endl;
    nested_value->dump("| " + indent);
}

FunctionCallExpression::FunctionCallExpression(
    std::unique_ptr<Expression> callee,
    vector<std::unique_ptr<Expression>>&& positional_arguments,
    unordered_map<string, std::unique_ptr<Expression>>&& keyword_arguments,
    string unparsed_config)
    : callee(std::move(callee))
    , positional_arguments(std::move(positional_arguments))
    , keyword_arguments(std::move(keyword_arguments))
    , unparsed_config(std::move(unparsed_config))
{
}

static std::unique_ptr<DecoratedExpression> decorate_and_convert(
    const Expression& expression,
    const plugins::Type& target_type,
    DecorateContext& context)
{
    auto [decorated_expression, type] = expression.decorate(context);

    if (*type != target_type) {
        TraceBlock block(context, "Adding casting expression");
        if (type->can_convert_into(target_type)) {
            return std::make_unique<DecoratedConvertExpression>(
                std::move(decorated_expression),
                *type,
                target_type);
        }

        context.error(
            "Cannot convert from type '{}' to type '{}'\n",
            type->name(),
            target_type.name());
    }

    return std::move(decorated_expression);
}

bool FunctionCallExpression::collect_argument(
    const Expression& arg,
    const plugins::ArgumentInfo& arg_info,
    DecorateContext& context,
    CollectedArguments& arguments,
    bool is_default)
{
    string key = arg_info.key;
    if (arguments.contains(key)) { return false; }

    std::unique_ptr<DecoratedExpression> decorated_arg =
        decorate_and_convert(arg, arg_info.type, context);

    if (arg_info.bounds.has_bound()) {
        std::unique_ptr<DecoratedExpression> decorated_min_expression;
        {
            TraceBlock block(context, "Handling lower bound");
            const std::unique_ptr<Expression> min_expression =
                parse_expression(arg_info.bounds.min, context);
            decorated_min_expression =
                decorate_and_convert(*min_expression, arg_info.type, context);
        }
        std::unique_ptr<DecoratedExpression> decorated_max_expression;
        {
            TraceBlock block(context, "Handling upper bound");
            const std::unique_ptr<Expression> max_expression =
                parse_expression(arg_info.bounds.max, context);
            decorated_max_expression =
                decorate_and_convert(*max_expression, arg_info.type, context);
        }
        decorated_arg = std::make_unique<DecoratedCheckBoundsExpression>(
            std::move(decorated_arg),
            std::move(decorated_min_expression),
            std::move(decorated_max_expression));
    }

    arguments.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(key),
        std::forward_as_tuple(std::move(decorated_arg), is_default));

    return true;
}

void FunctionCallExpression::collect_keyword_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments) const
{
    unordered_map<string, plugins::ArgumentInfo> argument_infos_by_key;
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        assert(!argument_infos_by_key.contains(arg_info.key));
        argument_infos_by_key.insert({arg_info.key, arg_info});
    }

    for (const auto& [key, arg] : keyword_arguments) {
        TraceBlock block(context, "Checking the keyword argument '{}'.", key);
        if (!argument_infos_by_key.contains(key)) {
            vector<string> valid_keys = get_keys<string>(argument_infos_by_key);

            context.error(
                "Unknown keyword argument: {}\n"
                "Valid keyword arguments are: {:n}\n",
                key,
                valid_keys);
        }
        const plugins::ArgumentInfo& arg_info = argument_infos_by_key.at(key);
        const bool success =
            collect_argument(*arg, arg_info, context, arguments, false);
        if (!success) {
            throw ContextError(
                "Multiple keyword definitions using the same key '{}'."
                "This should be impossible here because we sort by key "
                "earlier.",
                key);
        }
    }
}

/* This function has to be called *AFTER* collect_keyword_arguments. */
void FunctionCallExpression::collect_positional_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments) const
{
    // Check if too many arguments are specified for the plugin
    const std::size_t num_pos_args = positional_arguments.size();
    const std::size_t num_kwargs = keyword_arguments.size();

    if (num_pos_args + num_kwargs > argument_infos.size()) {
        vector<string> allowed_keys;
        allowed_keys.reserve(argument_infos.size());
        for (const auto& arg_info : argument_infos) {
            allowed_keys.push_back(arg_info.key);
        }
        vector<string> given_positional_keys;
        for (size_t i = 0; i < positional_arguments.size(); ++i) {
            if (i < argument_infos.size()) {
                given_positional_keys.push_back(argument_infos[i].key);
            } else {
                given_positional_keys.emplace_back("?");
            }
        }
        vector<string> given_keyword_keys = get_keys(keyword_arguments);

        context.error(
            "Too many parameters specified!\n"
            "Allowed parameters: {:n}\n"
            "Given positional parameters: {:n}\n"
            "Given keyword parameters: {:n}\n",
            allowed_keys,
            given_positional_keys,
            given_keyword_keys);
    }

    for (std::size_t i = 0; i < num_pos_args; ++i) {
        Expression& arg = *positional_arguments[i];
        const plugins::ArgumentInfo& arg_info = argument_infos[i];
        TraceBlock block(
            context,
            "Checking the {}. positional argument ({})",
            i + 1,
            arg_info.key);

        const bool success =
            collect_argument(arg, arg_info, context, arguments, false);

        if (!success) {
            context.error(
                "The argument '{}' is defined by the {}. positional argument "
                "and by a keyword argument.\n",
                arg_info.key,
                i + 1);
        }
    }
}

/* This function has to be called *AFTER* collect_positional_arguments. */
void FunctionCallExpression::collect_default_values(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments)
{
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        const string& key = arg_info.key;
        if (!arguments.contains(key)) {
            TraceBlock block(
                context,
                "Checking the default for argument '{}'.",
                key);

            if (arg_info.has_default()) {
                std::unique_ptr<Expression> arg;
                {
                    TraceBlock _(context, "Parsing default value");
                    arg = parse_expression(arg_info.default_value, context);
                }
                const bool success =
                    collect_argument(*arg, arg_info, context, arguments, true);
                if (!success) {
                    context.error(
                        "Default argument for '{}' set although "
                        "value for keyword exists. This should be impossible.",
                        key);
                }
            } else if (!arg_info.is_optional()) {
                context.error("Missing argument is mandatory!");
            }
        }
    }
}

TypedDecoratedExpressionPtr
FunctionCallExpression::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking Function Call");

    const IdentifierExpression* expression =
        dynamic_cast<IdentifierExpression*>(callee.get());

    if (!expression) { context.error("Callee is not a variable!"); }

    const std::string& name = expression->get_identifier().content;

    const plugins::Registry& registry = context.get_registry();
    if (!registry.has_feature(name)) {
        context.error("Plugin '{}' is not defined.", name);
    }

    shared_ptr<const plugins::Feature> feature = registry.get_feature(name);
    const vector<plugins::ArgumentInfo>& argument_infos =
        feature->get_arguments();

    CollectedArguments arguments_by_key;
    collect_keyword_arguments(argument_infos, context, arguments_by_key);
    collect_positional_arguments(argument_infos, context, arguments_by_key);
    collect_default_values(argument_infos, context, arguments_by_key);

    vector<std::pair<std::string, FunctionArgument>> arguments;
    arguments.reserve(arguments_by_key.size());

    for (auto& val : arguments_by_key) {
        arguments.emplace_back(std::move(val));
    }

    return {
        .expression = std::make_unique<DecoratedFunctionCallExpression>(
            feature,
            std::move(arguments),
            unparsed_config),
        .type = &feature->get_type()};
}

void FunctionCallExpression::dump(string indent) const
{
    cout << indent << "FUNC:" << endl;
    callee->dump("| " + indent);
    indent = "| " + indent;
    cout << indent << "POSITIONAL ARGS:" << endl;
    for (const std::unique_ptr<Expression>& expression : positional_arguments) {
        expression->dump("| " + indent);
    }
    cout << indent << "KEYWORD ARGS:" << endl;
    for (const auto& pair : keyword_arguments) {
        cout << indent << pair.first << " = " << endl;
        pair.second->dump("| " + indent);
    }
}

ListExpression::ListExpression(vector<std::unique_ptr<Expression>>&& elements)
    : elements(std::move(elements))
{
}

static const plugins::Type*
get_common_element_type(const std::vector<const plugins::Type*>& types)
{
    const plugins::Type* common_element_type = nullptr;
    for (const plugins::Type* element_type : types) {
        if (!common_element_type ||
            (!element_type->can_convert_into(*common_element_type) &&
             common_element_type->can_convert_into(*element_type))) {
            common_element_type = element_type;
        } else if (!element_type->can_convert_into(*common_element_type)) {
            return nullptr;
        }
    }
    return common_element_type;
}

TypedDecoratedExpressionPtr
ListExpression::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking list");
    vector<std::unique_ptr<DecoratedExpression>> decorated_elements;
    vector<const plugins::Type*> types;

    if (elements.empty()) {
        return {
            .expression = std::make_unique<DecoratedListExpression>(
                std::move(decorated_elements)),
            .type = &plugins::TypeRegistry::EMPTY_LIST_TYPE};
    }

    for (size_t i = 0; i < elements.size(); i++) {
        TraceBlock _(context, "Checking {}. element", i);
        TypedDecoratedExpressionPtr decorated_element_expression =
            elements[i]->decorate(context);
        decorated_elements.push_back(
            std::move(decorated_element_expression.expression));
        types.push_back(decorated_element_expression.type);
    }

    const plugins::Type* common_element_type = get_common_element_type(types);

    if (!common_element_type) {
        vector<string> element_type_names;
        element_type_names.reserve(elements.size());
        for (const plugins::Type* element_type : types) {
            element_type_names.push_back(element_type->name());
        }
        context.error(
            "List contains elements of different types: {:n}.",
            element_type_names);
    }

    for (size_t i = 0; i < elements.size(); i++) {
        if (const plugins::Type* element_type = types[i];
            element_type != common_element_type) {
            assert(element_type->can_convert_into(*common_element_type));
            std::unique_ptr<DecoratedExpression>& decorated_element_expression =
                decorated_elements[i];
            decorated_element_expression =
                std::make_unique<DecoratedConvertExpression>(
                    std::move(decorated_element_expression),
                    *element_type,
                    *common_element_type);
        }
    }

    return {
        .expression = std::make_unique<DecoratedListExpression>(
            std::move(decorated_elements)),
        .type = &plugins::TypeRegistry::instance()->create_list_type(
            *common_element_type)};
}

void ListExpression::dump(string indent) const
{
    cout << indent << "LIST:" << endl;
    indent = "| " + indent;
    for (const std::unique_ptr<Expression>& expression : elements) {
        expression->dump(indent);
    }
}

IdentifierExpression::IdentifierExpression(Token identifier)
    : identifier(std::move(identifier))
{
}

const Token& IdentifierExpression::get_identifier() const
{ return identifier; }

TypedDecoratedExpressionPtr
IdentifierExpression::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking Identifier: {}", identifier.content);
    if (const auto* td = context.get_typed_definition(identifier.content)) {
        if (identifier.type != TokenType::IDENTIFIER) {
            throw ContextError(
                "A non-identifier token was defined as variable.");
        }

        auto n =
            std::make_unique<DecoratedIdentifierExpression>(*td->definition);
        td->definition->usages.push_back(n.get());
        return {.expression = std::move(n), .type = td->type};
    }

    return {
        .expression =
            std::make_unique<DecoratedSymbolExpression>(identifier.content),
        .type = &plugins::TypeRegistry::SYMBOL_TYPE};
}

void IdentifierExpression::dump(std::string indent) const
{
    cout << indent << token_type_name(identifier.type) << ": "
         << identifier.content << endl;
}

LiteralExpression::LiteralExpression(Token value)
    : value(std::move(value))
{
}

TypedDecoratedExpressionPtr
LiteralExpression::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking Literal: {}", value.content);

    switch (value.type) {
    case TokenType::TRUE:
        return {
            .expression =
                std::make_unique<DecoratedBoolLiteralExpression>(true),
            .type = &plugins::TypeRegistry::instance()->get_type<bool>()};
    case TokenType::FALSE:
        return {
            .expression =
                std::make_unique<DecoratedBoolLiteralExpression>(false),
            .type = &plugins::TypeRegistry::instance()->get_type<bool>()};
    case TokenType::STRING:
        return {
            .expression = std::make_unique<DecoratedStringLiteralExpression>(
                value.content),
            .type = &plugins::TypeRegistry::instance()->get_type<string>()};
    case TokenType::INTEGER:
        return {
            .expression =
                std::make_unique<DecoratedIntLiteralExpression>(value.content),
            .type = &plugins::TypeRegistry::instance()->get_type<int>()};
    case TokenType::FLOAT:
        return {
            .expression = std::make_unique<DecoratedFloatLiteralExpression>(
                value.content),
            .type = &plugins::TypeRegistry::instance()->get_type<double>()};
    default:
        throw ContextError(
            "LiteralExpression has unexpected token type '{}'.",
            token_type_name(value.type));
    }
}

void LiteralExpression::dump(string indent) const
{
    cout << indent << token_type_name(value.type) << ": " << value.content
         << endl;
}

PrefixExpression::PrefixExpression(
    Token expr_operator,
    std::unique_ptr<Expression> operand)
    : expr_operator(std::move(expr_operator))
    , operand(std::move(operand))
{
}

TypedDecoratedExpressionPtr
PrefixExpression::decorate(DecorateContext& context) const
{
    TraceBlock block(
        context,
        "Checking Prefix Expression: {}",
        expr_operator.content);

    auto [expression, type] = operand->decorate(context);

    switch (expr_operator.type) {
    case TokenType::PLUS:
    case TokenType::MINUS:
        if (type != &plugins::TypeRegistry::instance()->get_type<int>()) {
            throw ContextError(
                "Operand does not have type int.",
                token_type_name(expr_operator.type));
        }

        return {
            .expression = std::make_unique<DecoratedUnaryExpression>(
                expr_operator,
                std::move(expression)),
            .type = &plugins::TypeRegistry::instance()->get_type<int>()};
    default:
        throw ContextError(
            "Unary expression expression has unexpected token type '{}'.",
            token_type_name(expr_operator.type));
    }
}

void PrefixExpression::dump(string indent) const
{ cout << indent << "UNARY: " << expr_operator.content << endl; }

} // namespace language::parser
