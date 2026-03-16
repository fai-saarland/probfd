#include "language/typed_ast/decorated_function_call_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/plugins/options.h"
#include "language/plugins/types.h"

#include <any>
#include <functional>
#include <ranges>

using namespace std;

namespace language::parser {

FunctionArgument::FunctionArgument(
    std::unique_ptr<DecoratedASTNode> value,
    bool is_default)
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

DecoratedFunctionCallNode::DecoratedFunctionCallNode(
    std::unique_ptr<DecoratedASTNode> callee,
    vector<FunctionArgument> arguments,
    const string& unparsed_config)
    : callee(std::move(callee))
    , arguments(move(arguments))
    , unparsed_config(unparsed_config)
{
}

void DecoratedFunctionCallNode::remove_variable_usages()
{
    for (auto& arg : arguments) { arg.get_value().remove_variable_usages(); }
}

using FType = std::function<std::any(const plugins::Options&, const Context&)>;

std::any DecoratedFunctionCallNode::construct(ConstructContext& context) const
{
    TraceBlock cblock(context, "Constructing callee");
    const auto calleef = std::any_cast<FType>(callee->construct(context));

    plugins::Options opts(arguments.size());
    opts.set_unparsed_config(unparsed_config);
    for (std::size_t i = 0; const auto& arg : arguments) {
        TraceBlock block(context, "Constructing argument {}", i);
        opts.set(i++, arg.get_value().construct(context));
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
            ? static_cast<std::function<bool(const FunctionArgument&)>>(
                  [](const auto&) { return true; })
            : [](const auto& arg) { return !arg.is_default_argument(); };

    if (auto args = arguments | std::views::filter(filter); !args.empty()) {
        {
            const auto& arg = args.front();
            arg.get_value().print(out, 0, print_default_args);
        }

        for (const auto& arg : args | std::views::drop(1)) {
            arg.get_value().print(out, 0, print_default_args);
        }
    }

    std::print(out, ")");
}

} // namespace language::parser