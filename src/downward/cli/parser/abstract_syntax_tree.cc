#include "downward/cli/parser/abstract_syntax_tree.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/syntax_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/strings.h"

#include <cassert>
#include <charconv>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;

namespace downward::cli::parser {

struct VariableDefinition;

struct TypedDeclaration {
    const plugins::Type* type;
    VariableDeclaration* declaration;
};

class Scope {
    std::unique_ptr<Scope> parent = nullptr;
    std::unordered_map<std::string, TypedDeclaration> variables;

public:
    Scope() = default;

    explicit Scope(std::unique_ptr<Scope> parent)
        : parent(std::move(parent))
    {
    }

    std::unique_ptr<Scope>& get_parent() { return parent; }

    void
    insert(utils::Context& context, std::pair<string, TypedDeclaration> pair)
    {
        if (!variables.insert(pair).second) {
            context.error(
                "Variable '{}' is already defined in the current scope.",
                pair.first);
        }
    }

    bool insert(std::pair<string, TypedDeclaration> pair)
    {
        return variables.insert(pair).second;
    }

    bool has_variable(const string& name) const
    {
        return get_typed_declaration(name) != nullptr;
    }

    TypedDeclaration* get_typed_declaration(const string& name)
    {
        if (const auto it = variables.find(name); it != variables.end()) {
            return &it->second;
        }

        return parent ? parent->get_typed_declaration(name) : nullptr;
    }

    const TypedDeclaration* get_typed_declaration(const string& name) const
    {
        if (const auto it = variables.find(name); it != variables.end()) {
            return &it->second;
        }

        return parent ? parent->get_typed_declaration(name) : nullptr;
    }

    const plugins::Type& get_variable_type(const string& name)
    {
        const auto* t = get_typed_declaration(name);
        assert(t);
        return *t->type;
    }

    VariableDeclaration& get_variable_declaration(const string& name)
    {
        const auto* t = get_typed_declaration(name);
        assert(t);
        return *t->declaration;
    }
};

class VariableEnvironment {
    const plugins::Registry& registry;
    std::unique_ptr<Scope> scope;

public:
    explicit VariableEnvironment(const plugins::Registry& registry)
        : registry(registry)
        , scope(std::make_unique<Scope>())
    {
    }

    void add_variable(
        utils::Context& context,
        const string& name,
        const plugins::Type& type,
        VariableDeclaration& declaration)
    {
        scope->insert(context, {name, TypedDeclaration{&type, &declaration}});
    }

    bool add_variable(
        const string& name,
        const plugins::Type& type,
        VariableDeclaration& declaration)
    {
        return scope->insert({name, TypedDeclaration{&type, &declaration}});
    }

    bool has_variable(const string& name) const
    {
        return scope->has_variable(name);
    }

    const plugins::Type& get_variable_type(const string& name) const
    {
        return scope->get_variable_type(name);
    }

    VariableDeclaration& get_variable_definition(const string& name) const
    {
        return scope->get_variable_declaration(name);
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

static ASTNodePtr parse_ast_node(const string& definition)
{
    TokenStream tokens = split_tokens(definition);
    return parse(tokens);
}

TypeLiteralNode::TypeLiteralNode(const Token& value)
    : value(value)
{
}

const plugins::Type&
TypeLiteralNode::get_type(plugins::TypeRegistry& type_registry) const
{
    switch (value.type) {
    case TokenType::TYPE_BOOL: return type_registry.get_type<bool>();
    case TokenType::TYPE_INTEGER: return type_registry.get_type<int>();
    case TokenType::TYPE_FLOAT: return type_registry.get_type<float>();
    case TokenType::TYPE_STRING: return type_registry.get_type<std::string>();
    case TokenType::IDENTIFIER:
        throw new utils::CriticalError(
            "Type literals for user-defined types not implemented.");
    default: throw new utils::CriticalError("Unknown token: {}", value.content);
    }
}

void TypeLiteralNode::dump(std::string) const
{
}

DecoratedASTNodePtr ASTNode::decorate(const plugins::Registry& registry) const
{
    utils::Context context;
    VariableEnvironment env(registry);
    utils::TraceBlock block(context, "Start semantic analysis");
    return decorate(context, env).ast_node;
}

LetNode::LetNode(
    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions,
    ASTNodePtr nested_value)
    : variable_definitions(move(variable_definitions))
    , nested_value(move(nested_value))
{
}

TypedDecoratedAstNodePtr
LetNode::decorate(utils::Context& context, VariableEnvironment& env) const
{
    utils::TraceBlock lblock(
        context,
        "Checking Let: {:n:s}",
        variable_definitions | views::keys);

    std::vector<VariableDefinition> decorated_variable_definitions;
    decorated_variable_definitions.reserve(variable_definitions.size());

    env.enter_scope();

    for (const auto& [variable_name, variable_definition] :
         variable_definitions) {
        utils::TraceBlock block(context, "Check variable definition");
        auto [ast_node, type] = variable_definition->decorate(context, env);
        auto& declaration = decorated_variable_definitions.emplace_back(
            variable_name,
            std::move(ast_node));

        env.add_variable(context, variable_name, *type, declaration);
    }

    TypedDecoratedAstNodePtr decorated_nested_value;

    {
        utils::TraceBlock block(context, "Check nested expression.");
        decorated_nested_value = nested_value->decorate(context, env);
    }

    env.leave_scope();

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

TypedParameter::TypedParameter(
    std::string parameter_name,
    std::unique_ptr<TypeNode> type_node)
    : parameter_name(std::move(parameter_name))
    , type_node(std::move(type_node))
{
}

LambdaNode::LambdaNode(
    std::vector<TypedParameter> parameters,
    ASTNodePtr nested_value)
    : parameters(std::move(parameters))
    , nested_value(std::move(nested_value))
{
}

TypedDecoratedAstNodePtr
LambdaNode::decorate(utils::Context& context, VariableEnvironment& env) const
{
    utils::TraceBlock lblock(context, "Checking Lambda");

    std::vector<plugins::ArgumentInfo> arg_infos;
    VariableEnvironment nested_env(env.get_registry());

    std::vector<VariableDeclaration> decorated_variable_declarations;
    decorated_variable_declarations.reserve(parameters.size());

    std::size_t i = 1;
    for (auto& [variable_name, type_node] : parameters) {
        utils::TraceBlock block(context, "Checking Parameter #{}", i++);

        auto& param_declaration =
            decorated_variable_declarations.emplace_back(variable_name);
        const auto& t = type_node->get_type(*plugins::TypeRegistry::instance());
        arg_infos.emplace_back(
            plugins::ArgumentInfo::make_required(variable_name, t));
        const bool s =
            nested_env.add_variable(variable_name, t, param_declaration);

        if (!s) {
            context.error(
                "A parameter with name '{}' already exists.",
                variable_name);
        }
    }

    auto [ast_node, rtype] = [&] {
        utils::TraceBlock block(context, "Checking Body.");
        return nested_value->decorate(context, nested_env);
    }();

    const auto& ftype = plugins::TypeRegistry::instance()->create_function_type(
        *rtype,
        std::move(arg_infos));

    return {
        std::make_unique<DecoratedLambdaNode>(
            ftype,
            std::move(decorated_variable_declarations),
            std::move(ast_node)),
        &ftype};
}

void LambdaNode::dump(std::string indent) const
{
    cout << indent << "LAMBDA:";

    indent = "| " + indent;

    for (const auto& [variable_name, type_name] : parameters) {
        cout << variable_name << ":" << type_name << std::endl;
    }

    cout << indent << ":" << endl;
    nested_value->dump("| " + indent);
}

FunctionCallNode::FunctionCallNode(
    ASTNodePtr callee,
    vector<ASTNodePtr>&& positional_arguments,
    unordered_map<string, ASTNodePtr>&& keyword_arguments,
    const string& unparsed_config)
    : callee(std::move(callee))
    , positional_arguments(move(positional_arguments))
    , keyword_arguments(move(keyword_arguments))
    , unparsed_config(unparsed_config)
{
}

static DecoratedASTNodePtr decorate_and_convert(
    const ASTNode& node,
    const plugins::Type& target_type,
    utils::Context& context,
    VariableEnvironment& env)
{
    auto [ast_node, type] = node.decorate(context, env);

    if (*type != target_type) {
        utils::TraceBlock block(context, "Adding casting node");
        if (type->can_convert_into(target_type)) {
            return std::make_unique<ConvertNode>(
                move(ast_node),
                *type,
                target_type);
        }

        context.error(
            "Cannot convert from type '{}' to type '{}'\n",
            type->name(),
            target_type.name());
    }
    return std::move(ast_node);
}

bool FunctionCallNode::collect_argument(
    const ASTNode& arg,
    const plugins::ArgumentInfo& arg_info,
    utils::Context& context,
    VariableEnvironment& env,
    CollectedArguments& arguments,
    bool is_default)
{
    string key = arg_info.key;
    if (arguments.contains(key)) { return false; }

    DecoratedASTNodePtr decorated_arg =
        decorate_and_convert(arg, arg_info.type, context, env);

    arguments.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(key),
        std::forward_as_tuple(move(decorated_arg), is_default));

    return true;
}

void FunctionCallNode::collect_keyword_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    utils::Context& context,
    VariableEnvironment& env,
    CollectedArguments& arguments) const
{
    unordered_map<string, plugins::ArgumentInfo> argument_infos_by_key;
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        assert(!argument_infos_by_key.contains(arg_info.key));
        argument_infos_by_key.insert({arg_info.key, arg_info});
    }

    for (const auto& [key, arg] : keyword_arguments) {
        utils::TraceBlock block(
            context,
            "Checking the keyword argument '{}'.",
            key);
        if (auto it = argument_infos_by_key.find(key);
            it != argument_infos_by_key.end()) {
            const bool success = collect_argument(
                *arg,
                it->second,
                context,
                env,
                arguments,
                false);
            if (!success) {
                throw utils::CriticalError(
                    "Multiple keyword definitions using the same key '{}'."
                    "This should be impossible here because we sort by key "
                    "earlier.",
                    key);
            }
        } else {
            context.error(
                "Unknown keyword argument: {:?}\n"
                "Valid keyword arguments are: {:n:?}\n",
                key,
                get_keys<string>(argument_infos_by_key));
        }
    }
}

/* This function has to be called *AFTER* collect_keyword_arguments. */
void FunctionCallNode::collect_positional_arguments(
    const vector<plugins::ArgumentInfo>& argument_infos,
    utils::Context& context,
    VariableEnvironment& env,
    CollectedArguments& arguments) const
{
    // Check if too many arguments are specified for the plugin
    const int num_pos_args = positional_arguments.size();
    const int num_kwargs = keyword_arguments.size();

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
            "Allowed parameters: {:n:s}\n"
            "Given positional parameters: {:n:s}\n"
            "Given keyword parameters: {:n:s}\n",
            allowed_keys,
            given_positional_keys,
            given_keyword_keys);
    }

    for (int i = 0; i < num_pos_args; ++i) {
        ASTNode& arg = *positional_arguments[i];
        const plugins::ArgumentInfo& arg_info = argument_infos[i];
        utils::TraceBlock block(
            context,
            "Checking the positional argument at index {} ({})",
            i + 1,
            arg_info.key);
        const bool success =
            collect_argument(arg, arg_info, context, env, arguments, false);
        if (!success) {
            context.error(
                "The argument '{}' is defined by the positional argument at "
                "index {} and by a keyword argument.",
                arg_info.key,
                i + 1);
        }
    }
}

/* This function has to be called *AFTER* collect_positional_arguments. */
void FunctionCallNode::collect_default_values(
    const vector<plugins::ArgumentInfo>& argument_infos,
    utils::Context& context,
    VariableEnvironment& env,
    CollectedArguments& arguments)
{
    for (const plugins::ArgumentInfo& arg_info : argument_infos) {
        if (const string& key = arg_info.key; !arguments.contains(key)) {
            utils::TraceBlock dblock(
                context,
                "Checking the default for argument '{}'.",
                key);

            if (arg_info.has_default()) {
                ASTNodePtr arg = [&] {
                    utils::TraceBlock block(context, "Parsing default value");
                    return parse_ast_node(arg_info.default_value);
                }();

                const bool success = collect_argument(
                    *arg,
                    arg_info,
                    context,
                    env,
                    arguments,
                    true);

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
FunctionCallNode::decorate(utils::Context& context, VariableEnvironment& env)
    const
{
    utils::TraceBlock block(context, "Checking Call");

    auto [dast_node, type] = callee->decorate(context, env);
    if (!type->is_function_type()) {
        context.error("Callee does not have function type.");
    }

    const auto& ftype = *static_cast<const plugins::FunctionType*>(type);
    const auto& argument_infos = ftype.get_argument_infos();

    CollectedArguments arguments_by_key;
    collect_keyword_arguments(argument_infos, context, env, arguments_by_key);
    collect_positional_arguments(
        argument_infos,
        context,
        env,
        arguments_by_key);
    collect_default_values(argument_infos, context, env, arguments_by_key);

    vector<std::pair<std::string, FunctionArgument>> arguments;
    arguments.reserve(arguments_by_key.size());

    for (auto& val : arguments_by_key) { arguments.push_back(move(val)); }

    return {
        std::make_unique<DecoratedFunctionCallNode>(
            std::move(dast_node),
            move(arguments),
            unparsed_config),
        &ftype.get_return_type()};
}

void FunctionCallNode::dump(string indent) const
{
    cout << indent << "FUNC: ";
    callee->dump();
    cout << endl;
    indent = "| " + indent;
    cout << indent << "POSITIONAL ARGS:" << endl;
    for (const ASTNodePtr& node : positional_arguments) {
        node->dump("| " + indent);
    }
    cout << indent << "KEYWORD ARGS:" << endl;
    for (const auto& [key, default_arg] : keyword_arguments) {
        cout << indent << key << " = " << endl;
        default_arg->dump("| " + indent);
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

TypedDecoratedAstNodePtr
ListNode::decorate(utils::Context& context, VariableEnvironment& env) const
{
    utils::TraceBlock lblock(context, "Checking list");
    vector<DecoratedASTNodePtr> decorated_elements;
    vector<const plugins::Type*> types;

    if (elements.empty()) {
        return {
            std::make_unique<DecoratedListNode>(move(decorated_elements)),
            &plugins::TypeRegistry::EMPTY_LIST_TYPE};
    }

    for (size_t i = 0; i < elements.size(); i++) {
        utils::TraceBlock block(context, "Checking element {}", i);

        auto [ast_node, type] = elements[i]->decorate(context, env);
        decorated_elements.push_back(move(ast_node));
        types.push_back(type);
    }

    const plugins::Type* common_element_type = get_common_element_type(types);

    if (!common_element_type) {
        vector<string> element_type_names;
        element_type_names.reserve(elements.size());
        for (const plugins::Type* element_type : types) {
            element_type_names.push_back(element_type->name());
        }
        context.error(
            "List contains elements of different types: {}",
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

UnaryNode::UnaryNode(ASTNodePtr nested_expr, const TokenType& token_type)
    : nested_expr(std::move(nested_expr))
    , token_type(token_type)
{
}

TypedDecoratedAstNodePtr
UnaryNode::decorate(utils::Context& context, VariableEnvironment& env) const
{
    auto [ast_node, type] = nested_expr->decorate(context, env);

    if (type == &plugins::TypeRegistry::instance()->get_type<int>()) {
        return {
            std::make_unique<DecoratedUnaryExpressionNode<int>>(
                std::move(ast_node),
                token_type),
            type};
    }

    if (type == &plugins::TypeRegistry::instance()->get_type<double>()) {
        return {
            std::make_unique<DecoratedUnaryExpressionNode<double>>(
                std::move(ast_node),
                token_type),
            type};
    }

    context.error(
        "Operator of unary arithmetic expression is not of numeric type.");
}

void UnaryNode::dump(std::string indent) const
{
    cout << indent << token_type_name(token_type);
    nested_expr->dump("");
    cout << endl;
}

LiteralNode::LiteralNode(const Token& value)
    : value(value)
{
}

TypedDecoratedAstNodePtr
LiteralNode::decorate(utils::Context& context, VariableEnvironment& env) const
{
    utils::TraceBlock block(context, "Checking Literal: " + value.content);
    if (env.has_variable(value.content)) {
        if (value.type != TokenType::IDENTIFIER) {
            throw utils::CriticalError(
                "A non-identifier token was defined as variable.");
        }
        string variable_name = value.content;
        auto& def = env.get_variable_definition(variable_name);
        auto n = std::make_unique<VariableNode>(def);
        def.usages.push_back(n.get());
        return {std::move(n), &env.get_variable_type(variable_name)};
    }

    if (const auto& reg = env.get_registry(); reg.has_feature(value.content)) {
        const auto& f = reg.get_feature(value.content);
        auto n = std::make_unique<FeatureLiteralNode>(f);
        const auto& t = plugins::TypeRegistry::instance()->create_function_type(
            f->get_type(),
            f->get_arguments());
        return {std::move(n), &t};
    }

    switch (value.type) {
    case TokenType::TRUE: {
        return {
            std::make_unique<BoolLiteralNode>(true),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    }
    case TokenType::FALSE: {
        return {
            std::make_unique<BoolLiteralNode>(false),
            &plugins::TypeRegistry::instance()->get_type<bool>()};
    }
    case TokenType::STRING: {
        if (value.content.front() != '"' || value.content.back() != '"') {
            throw utils::CriticalError(
                "String literal value is not enclosed in quotation marks.");
        }

        /*
          We are not doing any further syntax checking. Escaped symbols other
          than
          \n will just ignore the escaping \ (e.g., \t is treated as t, not as a
          tab). Strings ending in \ will not produce an error but should be
          excluded by the previous steps.
        */
        string result;
        result.reserve(value.content.length() - 2);
        bool escaped = false;
        for (const char c : value.content.substr(1, value.content.size() - 2)) {
            if (escaped) {
                escaped = false;
                if (c == 'n') {
                    result += '\n';
                } else {
                    result += c;
                }
            } else if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }

        return {
            std::make_unique<StringLiteralNode>(std::move(result)),
            &plugins::TypeRegistry::instance()->get_type<string>()};
    }
    case TokenType::INTEGER: {
        std::string_view sv{value.content.begin(), value.content.end()};

        int x;
        const auto [data, ec] = std::from_chars(sv.begin(), sv.end(), x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw utils::CriticalError(
                "Could not parse integer literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error(
                "Integer value is out of range: '{}'.",
                value.content);
        default:
            if (data == sv.end()) {
                return {
                    std::make_unique<IntLiteralNode>(x),
                    &plugins::TypeRegistry::instance()->get_type<int>()};
            }

            const auto& registry = env.get_registry();
            const auto operator_fname = std::format(
                "__operator_int_{}__",
                std::string_view{data, sv.end()});

            if (!registry.has_feature(operator_fname)) {
                context.error(
                    "User-defined int literal function '{}' not found.",
                    operator_fname);
            }

            const auto& feature = registry.get_feature(operator_fname);

            std::vector<std::pair<std::string, FunctionArgument>> arguments;
            arguments.emplace_back(
                "value",
                FunctionArgument(std::make_unique<IntLiteralNode>(x), false));

            return {
                std::make_unique<DecoratedFunctionCallNode>(
                    std::make_unique<FeatureLiteralNode>(feature),
                    std::move(arguments),
                    ""),
                &feature->get_type()};
        }
    }
    case TokenType::FLOAT: {
        std::string_view sv{value.content.begin(), value.content.end()};

        double x;
        const auto [data, ec] = std::from_chars(sv.begin(), sv.end(), x);

        switch (ec) {
        case std::errc::invalid_argument:
            throw utils::CriticalError(
                "Could not parse float literal '{}'.",
                value.content);
        case std::errc::result_out_of_range:
            context.error("Float value is out of range: '{}'.", value.content);
        default:
            if (data == sv.end()) {
                return {
                    std::make_unique<FloatLiteralNode>(x),
                    &plugins::TypeRegistry::instance()->get_type<double>()};
            }

            const auto& registry = env.get_registry();
            const auto operator_fname = std::format(
                "__operator_float_{}__",
                string_view{data, sv.end()});

            if (!registry.has_feature(operator_fname)) {
                context.error(
                    "User-defined float literal function '{}' not found.",
                    operator_fname);
            }

            const auto& feature = registry.get_feature(operator_fname);

            std::vector<std::pair<std::string, FunctionArgument>> arguments;
            arguments.emplace_back(
                "value",
                FunctionArgument(std::make_unique<FloatLiteralNode>(x), false));

            return {
                std::make_unique<DecoratedFunctionCallNode>(
                    std::make_unique<FeatureLiteralNode>(feature),
                    std::move(arguments),
                    ""),
                &feature->get_type()};
        }
    }
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
