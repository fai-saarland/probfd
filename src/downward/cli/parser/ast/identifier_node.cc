#include "downward/cli/parser/ast/identifier_node.h"

#include "downward/cli/parser/ast/variable_environment.h"

#include "downward/cli/parser/decorated_abstract_syntax_tree.h"
#include "downward/cli/parser/token.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/strings.h"

#include <vector>

using namespace std;

namespace downward::cli::parser {

IdentifierNode::IdentifierNode(QualifiedName qualified_name)
    : qualified_name(std::move(qualified_name))
{
}

TypedDecoratedAstNodePtr IdentifierNode::static_analysis(
    utils::Context& context,
    VariableEnvironment& env) const
{
    utils::TraceBlock block(context, "Checking Identifier: {}", qualified_name);

    const auto& [qualification, name] = qualified_name;

    if (qualification.empty() && env.has_variable(name)) {
        auto& def = env.get_variable_definition(name);
        auto n = std::make_unique<VariableNode>(def);
        def.usages.push_back(n.get());
        return {std::move(n), &env.get_variable_type(name)};
    }

    if (const auto& n = env.get_registry().get_namespace(qualification);
        n.has_function(name)) {
        const auto& f = n.get_function_definition(name);
        auto node = std::make_unique<FeatureLiteralNode>(f);
        return {std::move(node), &f.get_type()};
    }

    if (!qualification.empty()) {
        context.error(
            "Undefined variable {}.{}",
            utils::join_view(qualification, "."),
            name);
    }

    return {
        std::make_unique<SymbolNode>(name),
        &plugins::TypeRegistry::SYMBOL_TYPE};
}

const QualifiedName& IdentifierNode::get_name() const
{
    return qualified_name;
}

} // namespace downward::cli::parser
