#include "language/ast/list_node.h"

#include "language/typed_ast/convert_node.h"
#include "language/typed_ast/decorated_list_node.h"

#include "language/ast/internal_function_definition.h"
#include "language/typed_ast/types.h"

#include "language/context.h"

#include <cassert>
#include <vector>

using namespace std;

namespace language::parser {

ListNode::ListNode(vector<std::unique_ptr<ExpressionNode>>&& elements)
    : elements(move(elements))
{
}

static const typed_ast::Type*
get_common_element_type(const std::vector<const typed_ast::Type*>& types)
{
    const typed_ast::Type* common_element_type = nullptr;
    for (const typed_ast::Type* element_type : types) {
        if ((!common_element_type) ||
            (!element_type->can_convert_into(*common_element_type) &&
             common_element_type->can_convert_into(*element_type))) {
            common_element_type = element_type;
        } else if (!element_type->can_convert_into(*common_element_type)) {
            return nullptr;
        }
    }
    return common_element_type;
}

TypedDecoratedAstNodePtr ListNode::static_analysis(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::LocalEnvironment& local_env,
    typed_ast::TypeRegistry& type_registry) const
{
    TraceBlock lblock(context, "Checking list");
    vector<std::unique_ptr<typed_ast::DecoratedExpressionNode>>
        decorated_elements;
    vector<const typed_ast::Type*> types;

    if (elements.empty()) {
        return {
            std::make_unique<typed_ast::DecoratedListNode>(
                move(decorated_elements)),
            &typed_ast::TypeRegistry::EMPTY_LIST_TYPE};
    }

    for (size_t i = 0; i < elements.size(); i++) {
        TraceBlock block(context, "Checking element {}", i);

        auto [ast_node, type] = elements[i]->static_analysis(
            context,
            env,
            local_env,
            type_registry);
        decorated_elements.push_back(move(ast_node));
        types.push_back(type);
    }

    const typed_ast::Type* common_element_type = get_common_element_type(types);

    if (!common_element_type) {
        vector<string> element_type_names;
        element_type_names.reserve(elements.size());
        for (const typed_ast::Type* element_type : types) {
            element_type_names.push_back(element_type->name());
        }
        context.error(
            "List contains elements of different types: {}",
            element_type_names);
    }

    for (size_t i = 0; i < elements.size(); i++) {
        if (const typed_ast::Type* element_type = types[i];
            element_type != common_element_type) {
            assert(element_type->can_convert_into(*common_element_type));
            std::unique_ptr<typed_ast::DecoratedExpressionNode>&
                decorated_element_node = decorated_elements[i];
            decorated_element_node = std::make_unique<typed_ast::ConvertNode>(
                move(decorated_element_node),
                *element_type,
                *common_element_type);
        }
    }

    return {
        std::make_unique<typed_ast::DecoratedListNode>(
            move(decorated_elements)),
        &type_registry.create_list_type(*common_element_type)};
}

} // namespace language::parser
