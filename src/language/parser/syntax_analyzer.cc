#include "language/parser/syntax_analyzer.h"

#include "language/parser/lexical_analyzer.h"
#include "language/parser/token_stream.h"

#include "language/context.h"

#include <functional>
#include <ranges>
#include <unordered_set>

using namespace std;

namespace language::parser {

class SyntaxAnalyzerContext : public Context {
    const TokenStream& tokens;
    const int lookahead;

public:
    SyntaxAnalyzerContext(TokenStream& tokens, int lookahead)
        : tokens(tokens)
        , lookahead(lookahead)
    {
    }

    virtual string decorate_block_name(const string& block_name) const override
    {
        ostringstream decorated_block_name;
        int pos = tokens.get_position();
        decorated_block_name << block_name << ": "
                             << tokens.str(pos, pos + lookahead);
        if (pos + lookahead < tokens.size()) decorated_block_name << " ...";
        return decorated_block_name.str();
    }

    [[noreturn]]
    virtual void error(const string& message) const override
    {
        ostringstream message_with_tokens;
        string all_tokens = tokens.str(0, tokens.size());
        string remaining_tokens =
            tokens.str(tokens.get_position(), tokens.size());
        message_with_tokens
            << all_tokens << endl
            << string(all_tokens.size() - remaining_tokens.size(), ' ') << "^"
            << endl
            << message;
        throw ContextError(str() + "\n\n" + message_with_tokens.str());
    }

    using Context::error;
};

static ASTNodePtr parse_node(TokenStream& tokens, SyntaxAnalyzerContext&);

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

static ASTNodePtr parse_let(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock block(context, "Parsing Let");
    tokens.pop(context, TokenType::LET);

    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions;

    for (;;) {
        auto& [variable_name, variable_definition] =
            variable_definitions.emplace_back();

        {
            TraceBlock block(context, "Parsing variable definition");
            variable_definition = parse_node(tokens, context);
        }
        {
            TraceBlock block(
                context,
                "Parsing 'as' after variable definition.");
            tokens.pop(context, TokenType::AS);
        }
        {
            TraceBlock block(context, "Parsing variable name");
            variable_name = tokens.pop(context, TokenType::IDENTIFIER).content;
        }

        {
            TraceBlock block(
                context,
                "Parsing comma or 'in' after variable definition.");

            if (const auto next = tokens.pop(context);
                next.type == TokenType::COMMA) {
                continue;
            } else if (next.type == TokenType::IN) {
                break;
            } else {
                context.error(
                    "Got token {}. Expected either comma or 'in'.",
                    next);
            }
        }
    }

    ASTNodePtr nested_value;
    {
        TraceBlock block(context, "Parsing nested expression of let");
        nested_value = parse_node(tokens, context);
    }

    return std::make_unique<LetNode>(
        move(variable_definitions),
        move(nested_value));
}

static void parse_sequence(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType terminal_token,
    const function<void()>& func)
{
    TraceBlock block(context, "Parsing sequence");
    int num_argument = 1;
    while (tokens.peek(context).type != terminal_token) {
        {
            TraceBlock block(context, "Parsing {}. argument", num_argument);
            func();
        }
        {
            TraceBlock block(
                context,
                "Parsing token after {}. argument",
                num_argument);
            const TokenType next_type = tokens.peek(context).type;
            if (next_type == terminal_token) {
                return;
            } else if (next_type == TokenType::COMMA) {
                tokens.pop(context);
                if (tokens.peek(context).type == terminal_token) {
                    context.error("Trailing commas are forbidden.");
                }
            } else {
                context.error(
                    "Read unexpected token type '{}'. Expected token types "
                    "'{}' or '{}'.",
                    next_type,
                    terminal_token,
                    TokenType::COMMA);
            }
        }
        num_argument++;
    }
}

static ASTNodePtr
parse_function(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    int initial_token_stream_index = tokens.get_position();
    TraceBlock block(context, "Parsing plugin");
    string plugin_name;
    {
        TraceBlock block(context, "Parsing plugin name");
        Token name_token = tokens.pop(context, TokenType::IDENTIFIER);
        plugin_name = name_token.content;
    }
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);
    vector<ASTNodePtr> positional_arguments;
    unordered_map<string, ASTNodePtr> keyword_arguments;
    {
        TraceBlock block(context, "Parsing plugin arguments");
        auto callback = [&]() -> void {
            parse_argument(
                tokens,
                positional_arguments,
                keyword_arguments,
                context);
        };
        parse_sequence(
            tokens,
            context,
            TokenType::CLOSING_PARENTHESIS,
            callback);
    }
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);
    string unparsed_config =
        tokens.str(initial_token_stream_index, tokens.get_position());
    return std::make_unique<FunctionCallNode>(
        plugin_name,
        move(positional_arguments),
        move(keyword_arguments),
        unparsed_config);
}

static unordered_set<TokenType> literal_tokens{
    TokenType::TRUE,
    TokenType::FALSE,
    TokenType::STRING,
    TokenType::INTEGER,
    TokenType::FLOAT,
    TokenType::IDENTIFIER};

static ASTNodePtr
parse_literal(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock block(context, "Parsing Literal");
    Token token = tokens.pop(context);
    if (!literal_tokens.contains(token.type)) {
        context.error("Token {} cannot be parsed as literal", token);
    }
    return std::make_unique<LiteralNode>(token);
}

static ASTNodePtr
parse_list(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock block(context, "Parsing List");
    tokens.pop(context, TokenType::OPENING_BRACKET);
    vector<ASTNodePtr> elements;
    {
        TraceBlock block(context, "Parsing list arguments");
        auto callback = [&]() -> void {
            elements.push_back(parse_node(tokens, context));
        };
        parse_sequence(tokens, context, TokenType::CLOSING_BRACKET, callback);
    }
    tokens.pop(context, TokenType::CLOSING_BRACKET);
    return std::make_unique<ListNode>(move(elements));
}

static vector<TokenType> parse_node_token_types = {
    TokenType::OPENING_BRACKET,
    TokenType::LET,
    TokenType::TRUE,
    TokenType::FALSE,
    TokenType::STRING,
    TokenType::INTEGER,
    TokenType::FLOAT,
    TokenType::IDENTIFIER};

static ASTNodePtr
parse_node(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock block(context, "Identify node type");
    Token token = tokens.peek(context);
    if (ranges::find(parse_node_token_types, token.type) ==
        parse_node_token_types.end()) {

        context.error(
            "Unexpected token '{}'. "
            "Expected any of the following token types: {:s}",
            token,
            std::views::join_with(
                parse_node_token_types | std::views::transform(token_type_name),
                ", "));
    }

    switch (token.type) {
    case TokenType::OPENING_BRACKET: return parse_list(tokens, context);
    case TokenType::LET: return parse_let(tokens, context);
    case TokenType::TRUE:
    case TokenType::FALSE:
    case TokenType::STRING:
    case TokenType::INTEGER:
    case TokenType::FLOAT: return parse_literal(tokens, context);
    case TokenType::IDENTIFIER:
        if (tokens.has_tokens(2) &&
            tokens.peek(context, 1).type == TokenType::OPENING_PARENTHESIS) {
            return parse_function(tokens, context);
        } else {
            return parse_literal(tokens, context);
        }
    default:
        context.error("Unknown token type '{}'.", token_type_name(token.type));
    }
}

ASTNodePtr parse(TokenStream& tokens)
{
    SyntaxAnalyzerContext context(tokens, 10);
    TraceBlock block(context, "Start Syntactical Parsing");
    if (!tokens.has_tokens(1)) { context.error("Input is empty"); }
    ASTNodePtr node = parse_node(tokens, context);
    if (tokens.has_tokens(1)) {
        context.error(
            "Syntax analysis terminated with unparsed tokens: {}",
            tokens.str(tokens.get_position(), tokens.size()));
    }
    return node;
}

ASTNodePtr tokenize_and_parse(const std::string& expression)
{
    TokenStream tokens = split_tokens(expression);
    return parse(tokens);
}

} // namespace language::parser