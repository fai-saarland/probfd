#include "language/typed_ast/decorated_lambda_node.h"

#include "language/ast/lambda_node.h"
#include "language/typed_ast/construct_context.h"
#include "language/typed_ast/decorated_variable_node.h"

#include "language/typed_ast/types.h"

#include <functional>
#include <ranges>

using namespace std;

namespace {
struct TypedName {
    const std::string& name;
    const language::typed_ast::Type& type;
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

namespace language::typed_ast {

std::unique_ptr<DecoratedExpressionNode>
LambdaParameterValue::create_load_node()
{
    return std::make_unique<VariableNode>(*this);
}

DecoratedLambdaNode::DecoratedLambdaNode(
    const FunctionType& type,
    std::vector<LambdaParameterValue> decorated_variable_declarations,
    std::unique_ptr<DecoratedExpressionNode> nested_value)
    : type(type)
    , decorated_variable_declarations(
          std::move(decorated_variable_declarations))
    , nested_value(std::move(nested_value))
{
}

DecoratedLambdaNode::~DecoratedLambdaNode() = default;

DecoratedLambdaNode::DecoratedLambdaNode(DecoratedLambdaNode&&) noexcept =
    default;

std::any DecoratedLambdaNode::construct(ConstructContext&) const
{
    std::function f = [&](ConstructContext& stack) -> std::any {
        return nested_value->construct(stack);
    };

    return f;
}

} // namespace language::typed_ast