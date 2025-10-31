#include "downward/cli/parser/syntax_analyzer.h"

#include "downward/cli/parser/lexical_analyzer.h"
#include "downward/cli/parser/token_stream.h"

#include "downward/utils/logging.h"

#include <functional>
#include <unordered_set>

using namespace std;

namespace downward::cli::parser {
class SyntaxAnalyzerContext : public utils::Context {
    const TokenStream& tokens;
    const int lookahead;

public:
    SyntaxAnalyzerContext(TokenStream& tokens, int lookahead)
        : tokens(tokens)
        , lookahead(lookahead)
    {
    }

    string decorate_block_name(const string& block_name) const override
    {
        ostringstream decorated_block_name;
        const int pos = tokens.get_position();
        decorated_block_name << block_name << ": "
                             << tokens.str(pos, pos + lookahead);
        if (pos + lookahead < tokens.size()) decorated_block_name << " ...";
        return decorated_block_name.str();
    }

    [[noreturn]]
    void error(const string& message) const override
    {
        ostringstream message_with_tokens;
        const string all_tokens = tokens.str(0, tokens.size());
        const string remaining_tokens =
            tokens.str(tokens.get_position(), tokens.size());
        message_with_tokens
            << all_tokens << endl
            << string(all_tokens.size() - remaining_tokens.size(), ' ') << "^"
            << endl
            << message;
        throw utils::ContextError(str() + "\n\n" + message_with_tokens.str());
    }

    template <typename... Args>
    [[noreturn]]
    void error(format_string<Args...> fmt, Args&&... args) const
    {
        error(std::format(fmt, std::forward<Args>(args)...));
    }
};

template <typename F, typename... Args>
static auto parse_block(
    const std::string& block_name,
    const F& parsing_function,
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    Args&&... args)
    requires std::invocable<F, TokenStream&, SyntaxAnalyzerContext&, Args&&...>
{
    utils::TraceBlock block(context, block_name);
    return parsing_function(tokens, context, std::forward<Args>(args)...);
}

static ASTNodePtr parse_node(TokenStream& tokens, SyntaxAnalyzerContext&);
static ASTNodePtr parse_node_in_block(
    const std::string& block_name,
    TokenStream& tokens,
    SyntaxAnalyzerContext&);

template <std::invocable<TokenStream&, SyntaxAnalyzerContext&> F>
static void parse_sequence(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType separator_token,
    TokenType terminal_token,
    const F& func)
{
    utils::TraceBlock block(context, "Parsing sequence");

    for (int i = 1; tokens.peek(context).type != terminal_token; ++i) {
        {
            utils::TraceBlock nblock(context, "Parsing {}. argument", i);
            func(tokens, context);
        }
        {
            utils::TraceBlock nblock(
                context,
                "Parsing token after {}. argument",
                i);

            if (const TokenType next_type = tokens.peek(context).type;
                next_type == terminal_token) {
                return;
            } else if (next_type == separator_token) {
                tokens.pop(context);
                if (tokens.peek(context).type == terminal_token) {
                    context.error("Trailing commas are forbidden.");
                }
            } else {
                context.error(
                    "Read unexpected token type '{}'. "
                    "Expected token types '{}' or '{}'",
                    next_type,
                    terminal_token,
                    separator_token);
            }
        }
    }
}

template <std::invocable<TokenStream&, SyntaxAnalyzerContext&> F>
static void parse_sequence_in_block(
    const std::string& block_name,
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType separator_token,
    TokenType terminal_token,
    const F& func)
{
    utils::TraceBlock block(context, block_name);
    parse_sequence(tokens, context, separator_token, terminal_token, func);
}

template <std::invocable<TokenStream&, SyntaxAnalyzerContext&> F>
    requires(!std::same_as<
             std::invoke_result_t<F, TokenStream&, SyntaxAnalyzerContext&>,
             void>)
static std::vector<
    std::invoke_result_t<F, TokenStream&, SyntaxAnalyzerContext&>>
parse_sequence(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType separator_token,
    TokenType terminal_token,
    const F& func)
{
    std::vector<std::invoke_result_t<F, TokenStream&, SyntaxAnalyzerContext&>>
        v;

    parse_sequence(
        tokens,
        context,
        separator_token,
        terminal_token,
        [&](TokenStream& t, SyntaxAnalyzerContext& c) {
            v.emplace_back(func(t, c));
        });

    return v;
}

template <std::invocable<TokenStream&, SyntaxAnalyzerContext&> F>
    requires(!std::same_as<
             std::invoke_result_t<F, TokenStream&, SyntaxAnalyzerContext&>,
             void>)
static std::vector<
    std::invoke_result_t<F, TokenStream&, SyntaxAnalyzerContext&>>
parse_sequence_in_block(
    const std::string& block_name,
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType separator_token,
    TokenType terminal_token,
    const F& func)
{
    utils::TraceBlock block(context, block_name);
    return parse_sequence(
        tokens,
        context,
        separator_token,
        terminal_token,
        func);
}

static void parse_argument(
    TokenStream& tokens,
    vector<ASTNodePtr>& positional_arguments,
    unordered_map<string, ASTNodePtr>& keyword_arguments,
    SyntaxAnalyzerContext& context)
{
    if (tokens.has_tokens(2) &&
        tokens.peek(context, 0).type == TokenType::IDENTIFIER &&
        tokens.peek(context, 1).type == TokenType::EQUALS) {
        string argument_name = tokens.pop(context).content;
        tokens.pop(context, TokenType::EQUALS);
        if (keyword_arguments.contains(argument_name)) {
            context.error(
                "Multiple definitions of the same keyword argument '{}'.",
                argument_name);
        }
        keyword_arguments[argument_name] = parse_node(tokens, context);
    } else {
        if (!keyword_arguments.empty()) {
            context.error(
                "Positional arguments have to be defined before "
                "any keyword arguments.");
        }
        positional_arguments.push_back(parse_node(tokens, context));
    }
}

static std::pair<std::string, ASTNodePtr>
parse_let_definition(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    std::pair<std::string, ASTNodePtr> p;

    p.second =
        parse_node_in_block("Parsing variable definition", tokens, context);

    tokens.pop(context, TokenType::AS);

    p.first = tokens.pop(context, TokenType::IDENTIFIER).content;

    return p;
}

static ASTNodePtr parse_let(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing Let");
    tokens.pop(context, TokenType::LET);
    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions =
        parse_sequence_in_block(
            "Parsing let definitions",
            tokens,
            context,
            TokenType::COMMA,
            TokenType::IN,
            parse_let_definition);
    tokens.pop(context, TokenType::IN);

    ASTNodePtr nested_value = parse_node_in_block(
        "Parsing nested expression of let",
        tokens,
        context);

    return std::make_unique<LetNode>(
        move(variable_definitions),
        move(nested_value));
}

static std::unique_ptr<TypeNode>
parse_type(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock nblock(context, "Parsing type");

    switch (Token t = tokens.pop(context); t.type) {
    case TokenType::TYPE_BOOL:
    case TokenType::TYPE_INTEGER:
    case TokenType::TYPE_STRING:
    case TokenType::TYPE_FLOAT: return std::make_unique<TypeLiteralNode>(t);
    case TokenType::IDENTIFIER: return std::make_unique<TypeLiteralNode>(t);
    default:
        throw utils::CriticalError(
            "Expected type, but got token of type '{}'.",
            token_type_name(t.type));
    }
}

static TypedParameter
parse_typed_parameter(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    auto var_name = tokens.pop(context, TokenType::IDENTIFIER).content;

    tokens.pop(context, TokenType::COLON);

    auto type_node = parse_type(tokens, context);

    return TypedParameter(std::move(var_name), std::move(type_node));
}

static ASTNodePtr
parse_lambda(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing Lambda");
    tokens.pop(context, TokenType::LAMBDA);
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);
    std::vector<TypedParameter> params = parse_sequence_in_block(
        "Parsing lambda parameter list",
        tokens,
        context,
        TokenType::COMMA,
        TokenType::CLOSING_PARENTHESIS,
        parse_typed_parameter);
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);
    tokens.pop(context, TokenType::COLON);

    ASTNodePtr nested_value =
        parse_node_in_block("Parsing lambda body", tokens, context);

    return std::make_unique<LambdaNode>(move(params), move(nested_value));
}

static ASTNodePtr parse_function(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    ASTNodePtr callee)
{
    utils::TraceBlock block(context, "Parsing function call");

    const int initial_token_stream_index = tokens.get_position();
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);

    vector<ASTNodePtr> positional_arguments;
    unordered_map<string, ASTNodePtr> keyword_arguments;
    {
        auto callback = [&](TokenStream& t, SyntaxAnalyzerContext& c) {
            parse_argument(t, positional_arguments, keyword_arguments, c);
        };
        parse_sequence_in_block(
            "Parsing function arguments",
            tokens,
            context,
            TokenType::COMMA,
            TokenType::CLOSING_PARENTHESIS,
            callback);
    }
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);

    string unparsed_config =
        tokens.str(initial_token_stream_index, tokens.get_position());
    return std::make_unique<FunctionCallNode>(
        std::move(callee),
        move(positional_arguments),
        move(keyword_arguments),
        unparsed_config);
}

static constexpr std::array literal_tokens{
    TokenType::BOOLEAN,
    TokenType::STRING,
    TokenType::INTEGER,
    TokenType::FLOAT,
    TokenType::DURATION,
    TokenType::IDENTIFIER};

static ASTNodePtr
parse_literal(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing Literal");

    Token token = tokens.pop(context);
    if (!std::ranges::binary_search(literal_tokens, token.type)) {
        context.error("Token {} cannot be parsed as literal", token);
    }
    return std::make_unique<LiteralNode>(token);
}

static ASTNodePtr parse_non_prefix_op_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    ASTNodePtr lhs)
{
    utils::TraceBlock block(context, "Parsing Infix or Postfix Expression");

    while (tokens.has_tokens(1) &&
           tokens.peek(context).type == TokenType::OPENING_PARENTHESIS) {
        lhs = parse_function(tokens, context, std::move(lhs));
    }

    return lhs;
}

static ASTNodePtr parse_non_prefix_op_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context)
{
    return parse_non_prefix_op_expression(
        tokens,
        context,
        parse_literal(tokens, context));
}

static ASTNodePtr
parse_list(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing List");
    tokens.pop(context, TokenType::OPENING_BRACKET);
    vector<ASTNodePtr> elements = parse_sequence_in_block(
        "Parsing list arguments",
        tokens,
        context,
        TokenType::COMMA,
        TokenType::CLOSING_BRACKET,
        parse_node);
    tokens.pop(context, TokenType::CLOSING_BRACKET);
    return std::make_unique<ListNode>(move(elements));
}

static ASTNodePtr parse_parenthesized_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing parenthesized expression");
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);
    auto exp = parse_node(tokens, context);
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);
    return exp;
}

static ASTNodePtr
parse_node(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Identify node type");

    switch (const Token token = tokens.peek(context); token.type) {
    case TokenType::OPENING_PARENTHESIS:
        return parse_parenthesized_expression(tokens, context);
    case TokenType::OPENING_BRACKET: return parse_list(tokens, context);
    case TokenType::LET: return parse_let(tokens, context);
    case TokenType::LAMBDA: return parse_lambda(tokens, context);
    case TokenType::BOOLEAN:
    case TokenType::STRING:
    case TokenType::INTEGER:
    case TokenType::FLOAT:
    case TokenType::DURATION: return parse_literal(tokens, context);
    case TokenType::IDENTIFIER:
        return parse_non_prefix_op_expression(tokens, context);
    default:
        context.error(
            "Unexpected token '{}'. "
            "Expected any of the following token types: {}",
            token,
            std::array{
                TokenType::OPENING_PARENTHESIS,
                TokenType::OPENING_BRACKET,
                TokenType::LET,
                TokenType::LAMBDA,
                TokenType::BOOLEAN,
                TokenType::STRING,
                TokenType::INTEGER,
                TokenType::FLOAT,
                TokenType::DURATION,
                TokenType::IDENTIFIER});
    }
}

static ASTNodePtr parse_node_in_block(
    const std::string& block_name,
    TokenStream& tokens,
    SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, block_name);
    return parse_node(tokens, context);
}

ASTNodePtr parse(TokenStream& tokens)
{
    SyntaxAnalyzerContext context(tokens, 10);
    utils::TraceBlock block(context, "Start Syntactical Parsing");
    if (!tokens.has_tokens(1)) { context.error("Input is empty"); }
    ASTNodePtr node = parse_node(tokens, context);
    if (tokens.has_tokens(1)) {
        context.error(
            "Syntax analysis terminated with unparsed tokens: " +
            tokens.str(tokens.get_position(), tokens.size()));
    }
    return node;
}

ASTNodePtr tokenize_and_parse(const std::string& expression)
{
    TokenStream tokens = split_tokens(expression);
    return parse(tokens);
}

} // namespace downward::cli::parser