#include "language/typed_ast/decorated_let_node.h"

#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/variable_definition.h"

#include "language/plugins/types.h"

#include "downward/utils/logging.h"

#include <functional>
#include <ranges>

using namespace std;

namespace downward::cli::parser {

DecoratedLetNode::DecoratedLetNode(
    std::vector<VariableDefinition> decorated_variable_definitions,
    DecoratedASTNodePtr nested_value)
    : decorated_variable_definitions(move(decorated_variable_definitions))
    , nested_value(move(nested_value))
{
}

DecoratedLetNode::~DecoratedLetNode() = default;

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

} // namespace downward::cli::parser