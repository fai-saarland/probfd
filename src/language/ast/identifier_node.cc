#include "language/ast/identifier_node.h"

#include "language/ast/variable_environment.h"

#include "language/typed_ast/decorated_feature_literal_node.h"
#include "language/typed_ast/decorated_variable_node.h"
#include "language/typed_ast/symbol_node.h"
#include "language/typed_ast/variable_definition.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"
#include "language/plugins/types.h"

#include "language/context.h"

#include <vector>

using namespace std;

namespace language::parser {

IdentifierNode::IdentifierNode(QualifiedName qualified_name)
    : qualified_name(std::move(qualified_name))
{
}

TypedDecoratedAstNodePtr IdentifierNode::static_analysis(
    Context& context,
    VariableEnvironment& env) const
{
    TraceBlock block(context, "Checking Identifier: {}", qualified_name);

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
        context.error("Undefined variable", qualified_name);
    }

    return {
        std::make_unique<SymbolNode>(name),
        &plugins::TypeRegistry::SYMBOL_TYPE};
}

const QualifiedName& IdentifierNode::get_name() const
{
    return qualified_name;
}

} // namespace language::parser
