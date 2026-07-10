#include "language/parser/abstract_syntax_tree.h"

#include "language/parser/lexical_analyzer.h"
#include "language/parser/syntax_analyzer.h"
#include "language/parser/token_stream.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"
#include "language/plugins/types.h"

#include <cassert>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <vector>

using namespace std;

namespace language::parser {

struct VariableDefinition;

struct TypedDefinition {
    const plugins::Type* type;
    VariableDefinition* definition;
};

class Scope {
    std::unique_ptr<Scope> parent = nullptr;
    unordered_map<string, TypedDefinition> variables;

public:
    Scope() = default;

    explicit Scope(std::unique_ptr<Scope> parent)
        : parent(std::move(parent))
    {
    }

    std::unique_ptr<Scope>& get_parent() { return parent; }

    void insert(Context& context, std::pair<string, TypedDefinition> pair)
    {
        if (!variables.insert(pair).second) {
            context.error(
                "Variable '{}' is already defined in the current scope.",
                pair.first);
        }
    }

    bool has_variable(const string& name) const
    {
        return get_typed_definition(name) != nullptr;
    }

    TypedDefinition* get_typed_definition(const string& name)
    {
        const auto it = variables.find(name);
        if (it == variables.end()) {
            return parent ? parent->get_typed_definition(name) : nullptr;
        }

        return &it->second;
    }

    const TypedDefinition* get_typed_definition(const string& name) const
    {
        auto it = variables.find(name);
        if (it == variables.end()) {
            return parent ? parent->get_typed_definition(name) : nullptr;
        }

        return &it->second;
    }

    const plugins::Type& get_variable_type(const string& name)
    {
        const auto* t = get_typed_definition(name);
        assert(t);
        return *t->type;
    }

    VariableDefinition& get_variable_definition(const string& name)
    {
        const auto* t = get_typed_definition(name);
        assert(t);
        return *t->definition;
    }
};

class DecorateContext : public Context {
    const plugins::Registry registry;
    std::unique_ptr<Scope> scope;

public:
    explicit DecorateContext(const plugins::RawRegistry& raw_registry)
        : registry(raw_registry.construct_registry())
        , scope(std::make_unique<Scope>())
    {
    }

    void add_variable(
        const string& name,
        const plugins::Type& type,
        VariableDefinition& definition)
    {
        scope->insert(*this, {name, TypedDefinition{&type, &definition}});
    }

    bool has_variable(const string& name) const
    {
        return scope->has_variable(name);
    }

    const plugins::Type& get_variable_type(const string& name) const
    {
        return scope->get_variable_type(name);
    }

    VariableDefinition& get_variable_definition(const string& name) const
    {
        return scope->get_variable_definition(name);
    }

    void enter_scope() { scope = std::make_unique<Scope>(std::move(scope)); }

    void leave_scope() { scope = std::move(scope->get_parent()); }

    const plugins::Registry& get_registry() const { return registry; }
};

template <typename T, typename K>
static vector<T> get_keys(const unordered_map<T, K>& map)
{
    vector<T> keys;
    keys.reserve(map.size());
    for (const auto& key_value : map) { keys.push_back(key_value.first); }
    return keys;
}

static ASTNodePtr parse_ast_node(const string& definition, DecorateContext&)
{
    TokenStream tokens = split_tokens(definition);
    return parse(tokens);
}

DecoratedASTNodePtr
ASTNode::decorate(const plugins::RawRegistry& raw_registry) const
{
    DecorateContext context(raw_registry);
    TraceBlock block(context, "Start semantic analysis");
    return decorate(context).ast_node;
}

LetNode::LetNode(
    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions,
    ASTNodePtr nested_value)
    : variable_definitions(move(variable_definitions))
    , nested_value(move(nested_value))
{
}

TypedDecoratedAstNodePtr LetNode::decorate(DecorateContext& context) const
{
    TraceBlock _(
        context,
        "Checking Let: {:n}",
        variable_definitions | views::keys);

    std::vector<VariableDefinition> decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    context.enter_scope();

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        TraceBlock _(context, "Check variable definition");
        auto [ast_node, type] = variable_definition->decorate(context);
        auto& definition = decorated_variable_definitions.emplace_back(
            variable_name,
            std::move(ast_node));

        context.add_variable(variable_name, *type, definition);
    }

    TypedDecoratedAstNodePtr decorated_nested_value;

    {
        TraceBlock _(context, "Check nested expression.");
        decorated_nested_value = nested_value->decorate(context);
    }

    context.leave_scope();

    return {
        std::make_unique<DecoratedLetNode>(
            move(decorated_variable_definitions),
            move(decorated_nested_value.ast_node)),
        decorated_nested_value.type};
}

void LetNode::dump(string indent) const
{
    cout << indent << "LET:";

    indent = "| " + indent;

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        cout << variable_name << " = " << endl;
        variable_definition->dump(indent);
    }

    cout << indent << "IN:" << endl;
    nested_value->dump("| " + indent);
}

FunctionCallNode::FunctionCallNode(
    const string& name,
    vector<ASTNodePtr>&& positional_arguments,
    unordered_map<string, ASTNodePtr>&& keyword_arguments,
    const string& unparsed_config)
    : name(name)
    , positional_arguments(move(positional_arguments))
    , keyword_arguments(move(keyword_arguments))
    , unparsed_config(unparsed_config)
{
}

static DecoratedASTNodePtr decorate_and_convert(
    ASTNode& node,
    const plugins::Type& target_type,
    DecorateContext& context)
{
    TypedDecoratedAstNodePtr decorated_node = node.decorate(context);

    if (*decorated_node.type != target_type) {
        TraceBlock block(context, "Adding casting node");
        if (decorated_node.type->can_convert_into(target_type)) {
            return std::make_unique<ConvertNode>(
                move(decorated_node.ast_node),
                *decorated_node.type,
                target_type);
        }

        context.error(
            "Cannot convert from type '{}' to type '{}'\n",
            decorated_node.type->name(),
            target_type.name());
    }

    return std::move(decorated_node.ast_node);
}

bool FunctionCallNode::collect_argument(
    ASTNode& arg,
    const plugins::ArgumentInfo& arg_info,
    DecorateContext& context,
    CollectedArguments& arguments,
    bool is_default)
{
    string key = arg_info.key;
    if (arguments.contains(key)) { return false; }

    DecoratedASTNodePtr decorated_arg =
        decorate_and_convert(arg, arg_info.type, context);

    if (arg_info.bounds.has_bound()) {
        DecoratedASTNodePtr decorated_min_node;
        {
            TraceBlock block(context, "Handling lower bound");
            ASTNodePtr min_node = parse_ast_node(arg_info.bounds.min, context);
            decorated_min_node =
                decorate_and_convert(*min_node, arg_info.type, context);
        }
        DecoratedASTNodePtr decorated_max_node;
        {
            TraceBlock block(context, "Handling upper bound");
            ASTNodePtr max_node = parse_ast_node(arg_info.bounds.max, context);
            decorated_max_node =
                decorate_and_convert(*max_node, arg_info.type, context);
        }
        decorated_arg = std::make_unique<CheckBoundsNode>(
            move(decorated_arg),
            move(decorated_min_node),
            move(decorated_max_node));
    }

    arguments.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(key),
        std::forward_as_tuple(
            move(decorated_arg),
            is_default,
            arg_info.lazy_construction));

    return true;
}

void FunctionCallNode::collect_keyword_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments) const
{
    unordered_map<string, plugins::ArgumentInfo> argument_infos_by_key;
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        assert(!argument_infos_by_key.contains(arg_info.key));
        argument_infos_by_key.insert({arg_info.key, arg_info});
    }

    for (const auto& [key, arg] : keyword_arguments) {
        TraceBlock block(context, "Checking the keyword argument '{}'.", key);
        if (!argument_infos_by_key.contains(key)) {
            vector<string> valid_keys = get_keys<string>(argument_infos_by_key);

            context.error(
                "Unknown keyword argument: {}\n"
                "Valid keyword arguments are: {:n}\n",
                key,
                valid_keys);
        }
        const plugins::ArgumentInfo& arg_info = argument_infos_by_key.at(key);
        const bool success =
            collect_argument(*arg, arg_info, context, arguments, false);
        if (!success) {
            throw ContextError(
                "Multiple keyword definitions using the same key '{}'."
                "This should be impossible here because we sort by key "
                "earlier.",
                key);
        }
    }
}

/* This function has to be called *AFTER* collect_keyword_arguments. */
void FunctionCallNode::collect_positional_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments) const
{
    // Check if too many arguments are specified for the plugin
    int num_pos_args = positional_arguments.size();
    int num_kwargs = keyword_arguments.size();
    if (num_pos_args + num_kwargs > static_cast<int>(argument_infos.size())) {
        vector<string> allowed_keys;
        allowed_keys.reserve(argument_infos.size());
        for (const auto& arg_info : argument_infos) {
            allowed_keys.push_back(arg_info.key);
        }
        vector<string> given_positional_keys;
        for (size_t i = 0; i < positional_arguments.size(); ++i) {
            if (i < argument_infos.size()) {
                given_positional_keys.push_back(argument_infos[i].key);
            } else {
                given_positional_keys.push_back("?");
            }
        }
        vector<string> given_keyword_keys = get_keys(keyword_arguments);

        context.error(
            "Too many parameters specified!\n"
            "Allowed parameters: {:n}\n"
            "Given positional parameters: {:n}\n"
            "Given keyword parameters: {:n}\n",
            allowed_keys,
            given_positional_keys,
            given_keyword_keys);
    }

    for (int i = 0; i < num_pos_args; ++i) {
        ASTNode& arg = *positional_arguments[i];
        const plugins::ArgumentInfo& arg_info = argument_infos[i];
        TraceBlock block(
            context,
            "Checking the {}. positional argument ({})",
            i + 1,
            arg_info.key);

        const bool success =
            collect_argument(arg, arg_info, context, arguments, false);

        if (!success) {
            context.error(
                "The argument '{}' is defined by the {}. positional argument "
                "and by a keyword argument.\n",
                arg_info.key,
                i + 1);
        }
    }
}

/* This function has to be called *AFTER* collect_positional_arguments. */
void FunctionCallNode::collect_default_values(
    const vector<plugins::ArgumentInfo>& argument_infos,
    DecorateContext& context,
    CollectedArguments& arguments)
{
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        const string& key = arg_info.key;
        if (!arguments.contains(key)) {
            TraceBlock block(
                context,
                "Checking the default for argument '{}'.",
                key);

            if (arg_info.has_default()) {
                ASTNodePtr arg;
                {
                    TraceBlock block(context, "Parsing default value");
                    arg = parse_ast_node(arg_info.default_value, context);
                }
                const bool success =
                    collect_argument(*arg, arg_info, context, arguments, true);
                if (!success) {
                    context.error(
                        "Default argument for '{}' set although "
                        "value for keyword exists. This should be impossible.",
                        key);
                }
            } else if (!arg_info.is_optional()) {
                context.error("Missing argument is mandatory!");
            }
        }
    }
}

TypedDecoratedAstNodePtr
FunctionCallNode::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking Plugin: {}", name);
    const plugins::Registry& registry = context.get_registry();
    if (!registry.has_feature(name)) {
        context.error("Plugin '{}' is not defined.", name);
    }

    shared_ptr<const plugins::Feature> feature = registry.get_feature(name);
    const vector<plugins::ArgumentInfo>& argument_infos =
        feature->get_arguments();

    CollectedArguments arguments_by_key;
    collect_keyword_arguments(argument_infos, context, arguments_by_key);
    collect_positional_arguments(argument_infos, context, arguments_by_key);
    collect_default_values(argument_infos, context, arguments_by_key);

    vector<std::pair<std::string, FunctionArgument>> arguments;
    arguments.reserve(arguments_by_key.size());

    for (auto& val : arguments_by_key) { arguments.push_back(move(val)); }

    return {
        std::make_unique<DecoratedFunctionCallNode>(
            feature,
            move(arguments),
            unparsed_config),
        &feature->get_type()};
}

void FunctionCallNode::dump(string indent) const
{
    cout << indent << "FUNC:" << name << endl;
    indent = "| " + indent;
    cout << indent << "POSITIONAL ARGS:" << endl;
    for (const ASTNodePtr& node : positional_arguments) {
        node->dump("| " + indent);
    }
    cout << indent << "KEYWORD ARGS:" << endl;
    for (const auto& pair : keyword_arguments) {
        cout << indent << pair.first << " = " << endl;
        pair.second->dump("| " + indent);
    }
}

ListNode::ListNode(vector<ASTNodePtr>&& elements)
    : elements(move(elements))
{
}

static const plugins::Type*
get_common_element_type(const std::vector<const plugins::Type*>& types)
{
    const plugins::Type* common_element_type = nullptr;
    for (const plugins::Type* element_type : types) {
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

TypedDecoratedAstNodePtr ListNode::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking list");
    vector<DecoratedASTNodePtr> decorated_elements;
    vector<const plugins::Type*> types;

    if (elements.empty()) {
        return {
            std::make_unique<DecoratedListNode>(move(decorated_elements)),
            &plugins::TypeRegistry::EMPTY_LIST_TYPE};
    }
    for (size_t i = 0; i < elements.size(); i++) {
        TraceBlock block(context, "Checking {}. element", i);
        TypedDecoratedAstNodePtr decorated_element_node =
            elements[i]->decorate(context);
        decorated_elements.push_back(move(decorated_element_node.ast_node));
        types.push_back(decorated_element_node.type);
    }

    const plugins::Type* common_element_type = get_common_element_type(types);

    if (!common_element_type) {
        vector<string> element_type_names;
        element_type_names.reserve(elements.size());
        for (const plugins::Type* element_type : types) {
            element_type_names.push_back(element_type->name());
        }
        context.error(
            "List contains elements of different types: {:n}.",
            element_type_names);
    }

    for (size_t i = 0; i < elements.size(); i++) {
        if (const plugins::Type* element_type = types[i];
            element_type != common_element_type) {
            assert(element_type->can_convert_into(*common_element_type));
            DecoratedASTNodePtr& decorated_element_node = decorated_elements[i];
            decorated_element_node = std::make_unique<ConvertNode>(
                move(decorated_element_node),
                *element_type,
                *common_element_type);
        }
    }

    return {
        std::make_unique<DecoratedListNode>(move(decorated_elements)),
        &plugins::TypeRegistry::instance()->create_list_type(
            *common_element_type)};
}

void ListNode::dump(string indent) const
{
    cout << indent << "LIST:" << endl;
    indent = "| " + indent;
    for (const ASTNodePtr& node : elements) { node->dump(indent); }
}

LiteralNode::LiteralNode(const Token& value)
    : value(value)
{
}

TypedDecoratedAstNodePtr LiteralNode::decorate(DecorateContext& context) const
{
    TraceBlock block(context, "Checking Literal: {}", value.content);
    if (context.has_variable(value.content)) {
        if (value.type != TokenType::IDENTIFIER) {
            throw ContextError(
                "A non-identifier token was defined as variable.");
        }
        const string& variable_name = value.content;
        auto& def = context.get_variable_definition(variable_name);
        auto n = std::make_unique<VariableNode>(def);
        def.usages.push_back(n.get());
        return {std::move(n), &context.get_variable_type(variable_name)};
    }

    switch (value.type) {
    case TokenType::TRUE:
        return {
            std::make_unique<BoolLiteralNode>(true),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    case TokenType::FALSE:
        return {
            std::make_unique<BoolLiteralNode>(false),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    case TokenType::STRING:
        return {
            std::make_unique<StringLiteralNode>(value.content),
            &plugins::TypeRegistry::instance()->get_type<string>()};
    case TokenType::INTEGER:
        return {
            std::make_unique<IntLiteralNode>(value.content),
            &plugins::TypeRegistry::instance()->get_type<int>()};
    case TokenType::FLOAT:
        return {
            std::make_unique<FloatLiteralNode>(value.content),
            &plugins::TypeRegistry::instance()->get_type<double>()};
    case TokenType::IDENTIFIER:
        return {
            std::make_unique<SymbolNode>(value.content),
            &plugins::TypeRegistry::SYMBOL_TYPE};
    default:
        throw ContextError(
            "LiteralNode has unexpected token type '{}'.",
            token_type_name(value.type));
    }
}

void LiteralNode::dump(string indent) const
{
    cout << indent << token_type_name(value.type) << ": " << value.content
         << endl;
}

} // namespace language::parser
