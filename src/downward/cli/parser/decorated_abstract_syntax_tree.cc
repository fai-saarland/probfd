#include "downward/cli/parser/decorated_abstract_syntax_tree.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"

#include <any>
#include <cassert>
#include <functional>
#include <limits>
#include <ranges>

using namespace std;

namespace downward::cli::parser {

class ConstructContext : public utils::Context {
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

VariableDeclaration::VariableDeclaration(std::string variable_name)
    : variable_name(std::move(variable_name))
{
}

VariableDeclaration::VariableDeclaration(VariableDeclaration&& other) noexcept
    : variable_name(std::move(other.variable_name))
    , usages(std::move(other.usages))
{
    for (VariableNode* u : usages) { u->declaration = this; }
}

VariableDeclaration&
VariableDeclaration::operator=(VariableDeclaration&& other) noexcept
{
    variable_name = std::move(other.variable_name);
    usages = std::move(other.usages);

    for (VariableNode* u : usages) { u->declaration = this; }

    return *this;
}

VariableDefinition::VariableDefinition(
    std::string variable_name,
    DecoratedASTNodePtr variable_expression)
    : VariableDeclaration(std::move(variable_name))
    , variable_expression(std::move(variable_expression))
{
}

VariableDefinition::VariableDefinition(VariableDefinition&& other) noexcept
    : VariableDeclaration(std::move(other))
    , variable_expression(std::move(other.variable_expression))
{
}

VariableDefinition&
VariableDefinition::operator=(VariableDefinition&& other) noexcept
{
    VariableDeclaration::operator=(std::move(other));
    variable_expression = std::move(other.variable_expression);

    return *this;
}

std::vector<VariableDefinition> DecoratedASTNode::prune_unused_definitions()
{
    std::vector<VariableDefinition> defs;
    prune_unused_definitions(defs);
    return defs;
}

std::any DecoratedASTNode::construct() const
{
    ConstructContext context;
    utils::TraceBlock block(context, "Constructing parsed object");
    return construct(context);
}

FunctionArgument::FunctionArgument(DecoratedASTNodePtr value, bool is_default)
    : value(move(value))
    , is_default(is_default)
{
}

DecoratedASTNode& FunctionArgument::get_value()
{
    return *value;
}

const DecoratedASTNode& FunctionArgument::get_value() const
{
    return *value;
}

bool FunctionArgument::is_default_argument() const
{
    return is_default;
}

DecoratedLetNode::DecoratedLetNode(
    std::vector<VariableDefinition> decorated_variable_definitions,
    DecoratedASTNodePtr nested_value)
    : decorated_variable_definitions(move(decorated_variable_definitions))
    , nested_value(move(nested_value))
{
}

void DecoratedLetNode::prune_unused_definitions(
    std::vector<VariableDefinition>& defs)
{
    nested_value->prune_unused_definitions(defs);

    for (auto& def : std::views::reverse(decorated_variable_definitions)) {
        if (def.usages.empty()) {
            def.variable_expression->remove_variable_usages();
        }
    }

    auto [beg, end] = std::ranges::stable_partition(
        decorated_variable_definitions,
        [](const auto& def) { return !def.usages.empty(); });

    defs.insert(
        defs.end(),
        std::make_move_iterator(beg),
        std::make_move_iterator(end));

    decorated_variable_definitions.erase(beg, end);
}

void DecoratedLetNode::remove_variable_usages()
{
    for (const auto& def : decorated_variable_definitions) {
        def.variable_expression->remove_variable_usages();
    }
}

std::any DecoratedLetNode::construct(ConstructContext& context) const
{
    utils::TraceBlock lblock(context, "Constructing let-expression");

    for (const auto& def : decorated_variable_definitions) {
        utils::TraceBlock block(
            context,
            "Constructing variable '{}'",
            def.variable_name);

        context.set_variable(
            def.variable_name,
            def.variable_expression->construct(context));
    }

    std::any result = [&] {
        utils::TraceBlock block(context, "Constructing nested value");
        return nested_value->construct(context);
    }();

    for (const auto& def : decorated_variable_definitions) {
        context.remove_variable(def.variable_name);
    }

    return result;
}

void DecoratedLetNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{:{}}", "", indent);
    std::println(out, "let");

    if (!decorated_variable_definitions.empty()) {
        {
            const auto& def = decorated_variable_definitions.front();
            def.variable_expression->print(out, indent + 4, print_default_args);
            std::println(out, " as {}", def.variable_name);
        }

        for (const auto& def :
             decorated_variable_definitions | std::views::drop(1)) {
            std::println(out, ",");
            def.variable_expression->print(out, indent + 4, print_default_args);
            std::println(out, " as {}", def.variable_name);
        }
    }

    std::print(out, "{:{}}", "", indent);
    std::println(out, "in");

    nested_value->print(out, indent + 4, print_default_args);
}

DecoratedLambdaNode::DecoratedLambdaNode(
    const plugins::FunctionType& type,
    std::vector<VariableDeclaration> decorated_variable_declarations,
    DecoratedASTNodePtr nested_value)
    : type(type)
    , decorated_variable_declarations(
          std::move(decorated_variable_declarations))
    , nested_value(std::move(nested_value))
{
}

void DecoratedLambdaNode::prune_unused_definitions(
    std::vector<VariableDefinition>&)
{
}

void DecoratedLambdaNode::remove_variable_usages()
{
}

std::any DecoratedLambdaNode::construct(ConstructContext&) const
{
    std::function f = [&](const plugins::Options& opts,
                          const utils::Context&) -> std::any {
        ConstructContext nested_context;

        for (const auto& arg_info : type.get_argument_infos()) {
            nested_context.set_variable(
                arg_info.key,
                opts.get_raw(arg_info.key));
        }

        return nested_value->construct(nested_context);
    };

    return f;
}

void DecoratedLambdaNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "fun({:n:t}): ", type.get_argument_infos());
    nested_value->print(out, 0, print_default_args);
}

DecoratedFunctionCallNode::DecoratedFunctionCallNode(
    DecoratedASTNodePtr callee,
    vector<std::pair<std::string, FunctionArgument>>&& arguments,
    const string& unparsed_config)
    : callee(std::move(callee))
    , arguments(move(arguments))
    , unparsed_config(unparsed_config)
{
}

void DecoratedFunctionCallNode::remove_variable_usages()
{
    for (auto& arg : arguments | views::values) {
        arg.get_value().remove_variable_usages();
    }
}

using FType =
    std::function<std::any(const plugins::Options&, const utils::Context&)>;

std::any DecoratedFunctionCallNode::construct(ConstructContext& context) const
{
    utils::TraceBlock cblock(context, "Constructing callee");
    const auto calleef = std::any_cast<FType>(callee->construct(context));

    plugins::Options opts;
    opts.set_unparsed_config(unparsed_config);
    for (const auto& [key, arg] : arguments) {
        utils::TraceBlock block(context, "Constructing argument '{}'", key);
        opts.set(key, arg.get_value().construct(context));
    }
    return calleef(opts, context);
}

void DecoratedFunctionCallNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{}", std::string(indent, ' '));
    callee->print(out, 0, print_default_args);
    std::print(out, "(");

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

DecoratedListNode::DecoratedListNode(vector<DecoratedASTNodePtr>&& elements)
    : elements(move(elements))
{
}

void DecoratedListNode::remove_variable_usages()
{
    for (const auto& el : elements) { el->remove_variable_usages(); }
}

std::any DecoratedListNode::construct(ConstructContext& context) const
{
    utils::TraceBlock lblock(context, "Constructing list");
    vector<std::any> result;
    int i = 0;
    for (const DecoratedASTNodePtr& element : elements) {
        utils::TraceBlock block(context, "Constructing element {}", i++);
        result.push_back(element->construct(context));
    }
    return result;
}

void DecoratedListNode::print(
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

VariableNode::VariableNode(VariableDeclaration& declaration)
    : declaration(&declaration)
{
}

void VariableNode::remove_variable_usages()
{
    const auto it = std::ranges::find(declaration->usages, this);
    assert(it != declaration->usages.end());
    declaration->usages.erase(it);
}

std::any VariableNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Looking up variable '{}'",
        declaration->variable_name);

    if (!context.has_variable(declaration->variable_name)) {
        context.error(
            "Variable '{}' is not defined.",
            declaration->variable_name);
    }

    return context.get_variable(declaration->variable_name);
}

void VariableNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(
        out,
        "{:>{}}",
        declaration->variable_name,
        indent + declaration->variable_name.size());
}

FeatureLiteralNode::FeatureLiteralNode(
    std::shared_ptr<const plugins::Feature> feature)
    : feature(std::move(feature))
{
}

std::any FeatureLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing feature '{}'",
        feature->get_key());

    std::function f = [f = this->feature](
                          const plugins::Options& opts,
                          const utils::Context& ncontext) {
        return f->construct(opts, ncontext);
    };

    return f;
}

void FeatureLiteralNode::print(std::ostream& out, std::size_t indent, bool)
    const
{
    std::print(out, "{}{}", std::string(indent, ' '), feature->get_key());
}

BoolLiteralNode::BoolLiteralNode(bool value)
    : value(value)
{
}

std::any BoolLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing bool value from '{}'",
        value);

    return value;
}

void BoolLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

StringLiteralNode::StringLiteralNode(const string& value)
    : value(value)
{
}

std::any StringLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing string value from '{}'",
        value);

    return value;
}

void StringLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{:>{}?}", value, indent + value.size());
}

IntLiteralNode::IntLiteralNode(int value)
    : value(value)
{
}

std::any IntLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(context, "Constructing int value from '{}'", value);
    return value;
}

void IntLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

FloatLiteralNode::FloatLiteralNode(double value)
    : value(value)
{
}

std::any FloatLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing float value from '{}'",
        value);

    return value;
}

void FloatLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

DurationLiteralNode::DurationLiteralNode(utils::DynamicDuration value)
    : value(std::move(value))
{
}

std::any DurationLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing duration value from '{{{}, {}, {}}}'",
        value.num,
        value.denom,
        value.value);

    return value;
}

void DurationLiteralNode::print(std::ostream& out, std::size_t indent, bool)
    const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{{{}, {}, {}}}", value.num, value.denom, value.value);
}

SymbolNode::SymbolNode(const string& value)
    : value(value)
{
}

std::any SymbolNode::construct(ConstructContext&) const
{
    return std::any(value);
}

void SymbolNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

ConvertNode::ConvertNode(
    DecoratedASTNodePtr value,
    const plugins::Type& from_type,
    const plugins::Type& to_type)
    : value(move(value))
    , from_type(from_type)
    , to_type(to_type)
{
}

void ConvertNode::remove_variable_usages()
{
    value->remove_variable_usages();
}

std::any ConvertNode::construct(ConstructContext& context) const
{
    utils::TraceBlock cblock(
        context,
        "Constructing value that requires conversion");

    const std::any constructed_value = [&] {
        utils::TraceBlock block(
            context,
            "Constructing value of type '{}'",
            from_type.name());
        return value->construct(context);
    }();

    std::any converted_value = [&] {
        utils::TraceBlock block(
            context,
            "Converting constructed value from '{}' to '{}'",
            from_type.name(),
            to_type.name());
        return plugins::convert(constructed_value, from_type, to_type, context);
    }();

    return converted_value;
}

void ConvertNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

} // namespace downward::cli::parser