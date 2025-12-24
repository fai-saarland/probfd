#include "language/ast/type_identifier_node.h"

#include "language/plugins/types.h"

#include "language/context.h"
#include "language/typed_ast/variable_environment.h"

#ifdef _MSC_VER
// Disable "no return value" error, which is emitted despite Context::error
// being marked [[noreturn]]
#pragma warning(disable : 4716)
#endif

using namespace std;

namespace language::parser {

TypeIdentifierNode::TypeIdentifierNode(QualifiedName qualified_name)
    : qualified_name(std::move(qualified_name))
{
}

const plugins::Type& TypeIdentifierNode::get_type(
    Context& context,
    const VariableEnvironment& environment,
    plugins::TypeRegistry&) const
{
    const auto* type = environment.get_type(qualified_name.name);

    if (!type) {
        context.error("Type '{}' has not been defined.");
    }

    return *type;
}

} // namespace language::parser
