#include "language/ast/identifier_node.h"

#include "language/typed_ast/variable_environment.h"

#include "language/ast/internal_function_definition.h"

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
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry&) const
{
    TraceBlock block(context, "Checking Identifier: {}", qualified_name);

    const auto vnt_object =
        env.get_variable_declaration(local_env, qualified_name, context);

    return std::visit(
        utils::overload{
            [&](typed_ast::TypedValue object)
                -> TypedDecoratedAstNodePtr {
                return {object.declaration->create_load_node(), object.type};
            },
            [&](const std::vector<typed_ast::TypedFunctionValue>* object)
                -> TypedDecoratedAstNodePtr {
                if (object->size() > 1) {
                    context.error(
                        "Ambiguous variable '{}' refers to multiple function "
                        "overloads.",
                        qualified_name);
                }

                const auto& tdecl = object->front();
                return {tdecl.declaration->create_load_node(), tdecl.type};
            }},
        vnt_object);
}

const QualifiedName& IdentifierNode::get_name() const
{
    return qualified_name;
}

} // namespace language::parser
