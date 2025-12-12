#include "language/typed_ast/decorated_feature_literal_node.h"

#include "language/typed_ast/construct_context.h"

#include "language/plugins/options.h"
#include "language/plugins/plugin.h"

#include "language/context.h"

#include <any>
#include <functional>
#include <ranges>

using namespace std;

namespace language::parser {

FeatureLiteralNode::FeatureLiteralNode(
    const plugins::InternalFunctionDefinitionBase& feature)
    : feature(feature)
{
}

std::any FeatureLiteralNode::construct(ConstructContext& context) const
{
    TraceBlock block(
        context,
        "Constructing feature '{}'",
        feature.get_identifier());

    std::function f = [&f = this->feature](
                          const plugins::Options& opts,
                          const Context& ncontext) {
        return f.construct(opts, ncontext);
    };

    return f;
}

void FeatureLiteralNode::print(std::ostream& out, std::size_t indent, bool)
    const
{
    std::print(out, "{}{}", std::string(indent, ' '), feature.get_identifier());
}

} // namespace language::parser