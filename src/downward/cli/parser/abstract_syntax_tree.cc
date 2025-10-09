#include "downward/cli/parser/abstract_syntax_tree.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <unordered_map>
#include <vector>

using namespace std;

namespace downward::cli::parser {

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

    Scope(std::unique_ptr<Scope> parent)
        : parent(std::move(parent))
    {
    }

    std::unique_ptr<Scope>& get_parent() { return parent; }

    void
    insert(utils::Context& context, std::pair<string, TypedDefinition> pair)
    {
        if (!variables.insert(pair).second) {
            context.error(
                "Variable '" + pair.first +
                "' is already defined in the current scope.");
        }
    }

    bool has_variable(const string& name) const
    {
        return get_typed_definition(name) != nullptr;
    }

    TypedDefinition* get_typed_definition(const string& name)
    {
        auto it = variables.find(name);
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

class DecorateContext : public utils::Context {
    const plugins::Registry registry;
    std::unique_ptr<Scope> scope;

public:
    DecorateContext(const plugins::RawRegistry& raw_registry)
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
    utils::TraceBlock block(context, "Start semantic analysis");
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
    utils::TraceBlock block(
        context,
        "Checking Let: " +
            utils::join(variable_definitions | views::keys, ", "));

    std::vector<VariableDefinition> decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    context.enter_scope();

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        utils::TraceBlock block(context, "Check variable definition");
        auto [ast_node, type] = variable_definition->decorate(context);
        auto& definition = decorated_variable_definitions.emplace_back(
            variable_name,
            std::move(ast_node));

        context.add_variable(variable_name, *type, definition);
    }

    TypedDecoratedAstNodePtr decorated_nested_value;

    {
        utils::TraceBlock block(context, "Check nested expression.");
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
        utils::TraceBlock block(context, "Adding casting node");
        if (decorated_node.type->can_convert_into(target_type)) {
            return std::make_unique<ConvertNode>(
                move(decorated_node.ast_node),
                *decorated_node.type,
                target_type);
        } else {
            ostringstream message;
            message << "Cannot convert from type '"
                    << decorated_node.type->name() << "' to type '"
                    << target_type.name() << "'" << endl;
            context.error(message.str());
        }
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
    if (arguments.count(key)) { return false; }

    DecoratedASTNodePtr decorated_arg =
        decorate_and_convert(arg, arg_info.type, context);

    if (arg_info.bounds.has_bound()) {
        DecoratedASTNodePtr decorated_min_node;
        {
            utils::TraceBlock block(context, "Handling lower bound");
            ASTNodePtr min_node = parse_ast_node(arg_info.bounds.min, context);
            decorated_min_node =
                decorate_and_convert(*min_node, arg_info.type, context);
        }
        DecoratedASTNodePtr decorated_max_node;
        {
            utils::TraceBlock block(context, "Handling upper bound");
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
        std::forward_as_tuple(move(decorated_arg), is_default));

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

    for (const auto& key_and_arg : keyword_arguments) {
        const string& key = key_and_arg.first;
        ASTNode& arg = *key_and_arg.second;
        utils::TraceBlock block(
            context,
            "Checking the keyword argument '" + key + "'.");
        if (!argument_infos_by_key.count(key)) {
            vector<string> valid_keys = get_keys<string>(argument_infos_by_key);
            ostringstream message;
            message << "Unknown keyword argument: " << key << endl
                    << "Valid keyword arguments are: "
                    << utils::join(valid_keys, ", ") << endl;
            context.error(message.str());
        }
        const plugins::ArgumentInfo& arg_info = argument_infos_by_key.at(key);
        const bool success =
            collect_argument(arg, arg_info, context, arguments, false);
        if (!success) {
            throw utils::CriticalError(
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

        ostringstream message;
        message << "Too many parameters specified!" << endl
                << "Allowed parameters: " << utils::join(allowed_keys, ", ")
                << endl
                << "Given positional parameters: "
                << utils::join(given_positional_keys, ", ") << endl
                << "Given keyword parameters: "
                << utils::join(given_keyword_keys, ", ") << endl;
        context.error(message.str());
    }

    for (int i = 0; i < num_pos_args; ++i) {
        ASTNode& arg = *positional_arguments[i];
        const plugins::ArgumentInfo& arg_info = argument_infos[i];
        utils::TraceBlock block(
            context,
            "Checking the " + to_string(i + 1) + ". positional argument (" +
                arg_info.key + ")");
        bool success =
            collect_argument(arg, arg_info, context, arguments, false);
        if (!success) {
            ostringstream message;
            message << "The argument '" << arg_info.key
                    << "' is defined by the " << (i + 1)
                    << ". positional argument and by a keyword argument."
                    << endl;
            context.error(message.str());
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
        if (!arguments.count(key)) {
            utils::TraceBlock block(
                context,
                "Checking the default for argument '" + key + "'.");
            if (arg_info.has_default()) {
                ASTNodePtr arg;
                {
                    utils::TraceBlock block(context, "Parsing default value");
                    arg = parse_ast_node(arg_info.default_value, context);
                }
                const bool success =
                    collect_argument(*arg, arg_info, context, arguments, true);
                if (!success) {
                    throw utils::CriticalError(
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
    utils::TraceBlock block(context, "Checking Plugin: " + name);
    const plugins::Registry& registry = context.get_registry();
    if (!registry.has_feature(name)) {
        context.error("Plugin '" + name + "' is not defined.");
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
    utils::TraceBlock block(context, "Checking list");
    vector<DecoratedASTNodePtr> decorated_elements;
    vector<const plugins::Type*> types;

    if (elements.empty()) {
        return {
            std::make_unique<DecoratedListNode>(move(decorated_elements)),
            &plugins::TypeRegistry::EMPTY_LIST_TYPE};
    }
    for (size_t i = 0; i < elements.size(); i++) {
        utils::TraceBlock block(
            context,
            "Checking " + to_string(i) + ". element");
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
            "List contains elements of different types: [" +
            utils::join(element_type_names, ", ") + "].");
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
    utils::TraceBlock block(context, "Checking Literal: " + value.content);
    if (context.has_variable(value.content)) {
        if (value.type != TokenType::IDENTIFIER) {
            throw utils::CriticalError(
                "A non-identifier token was defined as variable.");
        }
        string variable_name = value.content;
        auto& def = context.get_variable_definition(variable_name);
        auto n = std::make_unique<VariableNode>(def);
        def.usages.push_back(n.get());
        return {std::move(n), &context.get_variable_type(variable_name)};
    }

    switch (value.type) {
    case TokenType::BOOLEAN:
        return {
            std::make_unique<BoolLiteralNode>(value.content),
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
    case TokenType::DURATION:
        return {
            std::make_unique<DurationLiteralNode>(value.content),
            &plugins::TypeRegistry::instance()
                 ->get_type<utils::DynamicDuration>()};
    case TokenType::IDENTIFIER:
        return {
            std::make_unique<SymbolNode>(value.content),
            &plugins::TypeRegistry::SYMBOL_TYPE};
    default:
        throw utils::CriticalError(
            "LiteralNode has unexpected token type '{}'.",
            token_type_name(value.type));
    }
}

void LiteralNode::dump(string indent) const
{
    cout << indent << token_type_name(value.type) << ": " << value.content
         << endl;
}

} // namespace downward::cli::parser
