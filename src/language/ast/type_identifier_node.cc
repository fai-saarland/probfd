#include "language/ast/type_identifier_node.h"

#include "language/plugins/types.h"

#include "language/context.h"

#ifdef _MSC_VER
// Disable "no return value" error, which is emitted despite Context::error
// being marked [[noreturn]]
#pragma warning(disable:4716)
#endif

using namespace std;

namespace language::parser {

TypeIdentifierNode::TypeIdentifierNode(QualifiedName name)
    : name(std::move(name))
{
}

const plugins::Type&
TypeIdentifierNode::get_type(Context& context, plugins::TypeRegistry&)
    const
{
    context.error("Static analysis for type identifiers not implemented.");
}

} // namespace language::parser
