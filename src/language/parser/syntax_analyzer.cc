#include "language/parser/syntax_analyzer.h"

#include "language/parser/abstract_syntax_tree.h"
#include "language/parser/lexical_analyzer.h"
#include "language/parser/token.h"
#include "language/parser/token_stream.h"

#include "language/context.h"

#include <functional>
#include <map>
#include <ranges>
#include <sstream>
#include <unordered_set>

using namespace std;

namespace language::parser {

namespace {

class SyntaxAnalyzerContext : public Context {
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
        int pos = tokens.get_position();
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
        throw ContextError(str() + "\n\n" + message_with_tokens.str());
    }

    using Context::error;
};

std::unique_ptr<Expression>
parse_expression(TokenStream& tokens, SyntaxAnalyzerContext&);

std::unique_ptr<Expression> parse_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    int precedence);

void parse_argument(
    TokenStream& tokens,
    vector<std::unique_ptr<Expression>>& positional_arguments,
    unordered_map<string, std::unique_ptr<Expression>>& keyword_arguments,
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
        keyword_arguments[argument_name] = parse_expression(tokens, context);
    } else {
        if (!keyword_arguments.empty()) {
            context.error(
                "Positional arguments have to be defined before "
                "any keyword arguments.");
        }
        positional_arguments.push_back(parse_expression(tokens, context));
    }
}

std::unique_ptr<Expression>
parse_let(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock _(context, "Parsing Let");
    tokens.pop(context);

    std::vector<std::pair<std::string, std::unique_ptr<Expression>>>
        variable_definitions;

    for (;;) {
        auto& [variable_name, variable_definition] =
            variable_definitions.emplace_back();

        {
            TraceBlock _(context, "Parsing variable definition");
            variable_definition = parse_expression(tokens, context);
        }
        {
            TraceBlock _(context, "Parsing 'as' after variable definition.");
            tokens.pop(context, TokenType::AS);
        }
        {
            TraceBlock _(context, "Parsing variable name");
            variable_name = tokens.pop(context, TokenType::IDENTIFIER).content;
        }

        {
            TraceBlock _(
                context,
                "Parsing comma or 'in' after variable definition.");

            const auto next = tokens.pop(context);

            if (next.type == TokenType::COMMA) { continue; }

            if (next.type == TokenType::IN) { break; }

            context.error("Got token {}. Expected either comma or 'in'.", next);
        }
    }

    std::unique_ptr<Expression> nested_value;
    {
        TraceBlock _(context, "Parsing nested expression of let");
        nested_value = parse_expression(tokens, context);
    }

    return std::make_unique<LetExpression>(
        std::move(variable_definitions),
        std::move(nested_value));
}

void parse_sequence(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType terminal_token,
    const function<void()>& func)
{
    TraceBlock _(context, "Parsing sequence");
    int num_argument = 1;
    while (tokens.peek(context).type != terminal_token) {
        {
            TraceBlock _(context, "Parsing {}. argument", num_argument);
            func();
        }
        {
            TraceBlock _(
                context,
                "Parsing token after {}. argument",
                num_argument);
            const TokenType next_type = tokens.peek(context).type;
            if (next_type == terminal_token) { return; }

            if (next_type == TokenType::COMMA) {
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

std::unique_ptr<Expression> parse_parenthesized_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context)
{
    TraceBlock _(context, "Parsing Literal");
    tokens.pop(context);
    auto expr = parse_expression(tokens, context, 0);
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);
    return expr;
}

std::unique_ptr<Expression>
parse_literal(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock _(context, "Parsing Literal");
    return std::make_unique<LiteralExpression>(tokens.pop(context));
}

std::unique_ptr<Expression>
parse_list(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock _(context, "Parsing List");
    tokens.pop(context);

    vector<std::unique_ptr<Expression>> elements;
    {
        TraceBlock _(context, "Parsing list arguments");
        auto callback = [&]() -> void {
            elements.push_back(parse_expression(tokens, context, 0));
        };
        parse_sequence(tokens, context, TokenType::CLOSING_BRACKET, callback);
    }

    tokens.pop(context, TokenType::CLOSING_BRACKET);

    return std::make_unique<ListExpression>(std::move(elements));
}

enum Precedence : int { PREFIX = 7, CALL = 8 };

std::unique_ptr<Expression>
identifier_parselet(TokenStream& tokens, SyntaxAnalyzerContext& context)
{ return std::make_unique<IdentifierExpression>(tokens.pop(context)); }

std::unique_ptr<Expression>
prefix_parselet(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    return std::make_unique<PrefixExpression>(
        tokens.pop(context),
        parse_expression(tokens, context, Precedence::PREFIX));
}

std::unique_ptr<Expression> function_call_parselet(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    std::unique_ptr<Expression> left)
{
    const int initial_token_stream_index = tokens.get_position();

    tokens.pop(context);

    vector<std::unique_ptr<Expression>> positional_arguments;
    unordered_map<string, std::unique_ptr<Expression>> keyword_arguments;
    {
        TraceBlock _(context, "Parsing plugin arguments");
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
    string unparsed_config =
        tokens.str(initial_token_stream_index, tokens.get_position());
    tokens.pop(context, TokenType::CLOSING_PARENTHESIS);

    return std::make_unique<FunctionCallExpression>(
        std::move(left),
        std::move(positional_arguments),
        std::move(keyword_arguments),
        unparsed_config);
}

using PrefixParselet = std::unique_ptr<Expression> (*)(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context);

struct InfixParselet {
    std::unique_ptr<Expression> (*parser)(
        TokenStream& tokens,
        SyntaxAnalyzerContext& context,
        std::unique_ptr<Expression> left);

    int precedence;
};

map<TokenType, PrefixParselet> prefix_parselets{
    {TokenType::OPENING_BRACKET, parse_list},
    {TokenType::OPENING_PARENTHESIS, parse_parenthesized_expression},
    {TokenType::LET, parse_let},
    {TokenType::TRUE, parse_literal},
    {TokenType::FALSE, parse_literal},
    {TokenType::STRING, parse_literal},
    {TokenType::INTEGER, parse_literal},
    {TokenType::FLOAT, parse_literal},
    {TokenType::IDENTIFIER, identifier_parselet},
    {TokenType::PLUS, prefix_parselet},
    {TokenType::MINUS, prefix_parselet}};

map<TokenType, InfixParselet> infix_parselets{
    {TokenType::OPENING_PARENTHESIS,
     {.parser = function_call_parselet,
      .precedence = std::to_underlying(Precedence::CALL)}}};

std::unique_ptr<Expression> parse_expression(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    int precedence)
{
    TraceBlock _(
        context,
        "Parsing expression at precedence level {}",
        precedence);

    Token token = tokens.peek(context);
    const auto prefix = prefix_parselets.find(token.type);

    if (prefix == prefix_parselets.end()) {
        context.error("Could not parse \"{}\".", token);
    }

    std::unique_ptr<Expression> left = prefix->second(tokens, context);

    while (tokens.has_tokens(1)) {
        auto infix = infix_parselets.find(tokens.peek(context).type);

        if (infix == infix_parselets.end() ||
            infix->second.precedence <= precedence) {
            break;
        }

        left = infix->second.parser(tokens, context, std::move(left));
    }

    return left;
}

std::unique_ptr<Expression>
parse_expression(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    TraceBlock _(context, "Parsing top-level expression");
    return parse_expression(tokens, context, 0);
}
} // namespace

std::unique_ptr<Expression> parse(TokenStream& tokens)
{
    SyntaxAnalyzerContext context(tokens, 10);
    TraceBlock _(context, "Start Syntactical Parsing");
    std::unique_ptr<Expression> node = parse_expression(tokens, context);
    if (tokens.has_tokens(1)) {
        context.error(
            "Syntax analysis terminated with unparsed tokens: {}",
            tokens.str(tokens.get_position(), tokens.size()));
    }
    return node;
}

std::unique_ptr<Expression> tokenize_and_parse(const std::string& expression)
{
    TokenStream tokens = split_tokens(expression);
    return parse(tokens);
}

} // namespace language::parser