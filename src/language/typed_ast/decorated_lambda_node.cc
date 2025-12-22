#include "language/typed_ast/decorated_lambda_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/decorated_variable_node.h"
#include "language/typed_ast/variable_declaration.h"

#include "language/plugins/options.h"
#include "language/plugins/types.h"

#include <functional>
#include <ranges>

using namespace std;

namespace {
struct TypedName {
    const std::string& name;
    const language::plugins::Type& type;
};
} // namespace

template <typename CharT>
struct std::formatter<TypedName, CharT> {
    template <typename FormatContext>
    constexpr auto parse(FormatContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const TypedName& tname, FormatContext& ctx) const
    {
        return std::format_to(
            ctx.out(),
            "{} : {}",
            tname.name,
            tname.type.name());
    }
};

namespace language::parser {

std::unique_ptr<DecoratedASTNode> LambdaParameter::create_load_node()
{
    auto node = std::make_unique<VariableNode>(*this);
    usages.push_back(node.get());
    return node;
}

DecoratedLambdaNode::DecoratedLambdaNode(
    const plugins::FunctionType& type,
    std::vector<LambdaParameter> decorated_variable_declarations,
    std::unique_ptr<DecoratedASTNode> nested_value)
    : type(type)
    , decorated_variable_declarations(
          std::move(decorated_variable_declarations))
    , nested_value(std::move(nested_value))
{
}

DecoratedLambdaNode::~DecoratedLambdaNode() = default;

DecoratedLambdaNode::DecoratedLambdaNode(DecoratedLambdaNode&&) noexcept =
    default;

void DecoratedLambdaNode::prune_unused_definitions(
    std::vector<std::unique_ptr<VariableDeclaration>>&)
{
}

void DecoratedLambdaNode::remove_variable_usages()
{
}

std::any DecoratedLambdaNode::construct(ConstructContext&) const
{
    std::function f = [&](const plugins::Options& opts,
                          const Context&) -> std::any {
        ConstructContext nested_context;

        for (std::size_t i = 0;
             const auto& arg_info : decorated_variable_declarations) {
            nested_context.set_variable(
                arg_info.variable_name,
                opts.get_raw(i++, nested_context));
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
    std::print(
        out,
        "fun({:n}): ",
        std::views::zip(
            decorated_variable_declarations,
            type.get_argument_types()) |
            std::views::transform([](const auto& t) {
                return TypedName{std::get<0>(t).variable_name, *std::get<1>(t)};
            }));
    nested_value->print(out, 0, print_default_args);
}

} // namespace language::parser