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
        throw utils::ContextError(str() + "\n\n" + message_with_tokens.str());
    }

    template <typename... Args>
    void error(format_string<Args...> fmt, Args&&... args) const
    {
        error(std::format(fmt, std::forward<Args>(args)...));
    }
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
        if (keyword_arguments.count(argument_name)) {
            context.error(
                "Multiple definitions of the same keyword "
                "argument '" +
                argument_name + "'.");
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
    utils::TraceBlock block(context, "Parsing Let");
    tokens.pop(context, TokenType::LET);

    std::vector<std::pair<std::string, ASTNodePtr>> variable_definitions;

    for (;;) {
        auto& [variable_name, variable_definition] =
            variable_definitions.emplace_back();

        {
            utils::TraceBlock block(context, "Parsing variable definition");
            variable_definition = parse_node(tokens, context);
        }
        {
            utils::TraceBlock block(
                context,
                "Parsing 'as' after variable definition.");
            tokens.pop(context, TokenType::AS);
        }
        {
            utils::TraceBlock block(context, "Parsing variable name");
            variable_name = tokens.pop(context, TokenType::IDENTIFIER).content;
        }

        {
            utils::TraceBlock block(
                context,
                "Parsing comma or 'in' after variable definition.");

            if (const auto next = tokens.pop(context);
                next.type == TokenType::COMMA) {
                continue;
            } else if (next.type == TokenType::IN) {
                break;
            } else {
                ostringstream message;
                message << "Got token " << next
                        << ". Expected either comma or 'in'.";
                context.error(message.str());
            }
        }
    }

    ASTNodePtr nested_value;
    {
        utils::TraceBlock block(context, "Parsing nested expression of let");
        nested_value = parse_node(tokens, context);
    }

    return std::make_unique<LetNode>(
        move(variable_definitions),
        move(nested_value));
}

static std::unique_ptr<TypeNode>
parse_type(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    switch (auto t = tokens.pop(context); t.type) {
    case TokenType::TYPE_BOOL:
    case TokenType::TYPE_INTEGER:
    case TokenType::TYPE_STRING:
    case TokenType::TYPE_FLOAT: return std::make_unique<TypeLiteralNode>(t);
    case TokenType::IDENTIFIER: return std::make_unique<TypeLiteralNode>(t);
    default:
        throw utils::CriticalError(
            "Expected type, but got hot token of type '{}'.",
            token_type_name(t.type));
    }
}

static ASTNodePtr
parse_lambda(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing Lambda");
    tokens.pop(context, TokenType::LAMBDA);
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);

    std::vector<TypedParameter> params;

    if (auto next = tokens.peek(context);
        next.type != TokenType::CLOSING_PARENTHESIS) {
        {
            auto var_name = [&] {
                utils::TraceBlock block(context, "Parsing argument name");
                return tokens.pop(context, TokenType::IDENTIFIER).content;
            }();

            tokens.pop(context, TokenType::COLON);

            auto type_node = [&] {
                utils::TraceBlock block(context, "Parsing type");
                return parse_type(tokens, context);
            }();

            params.emplace_back(std::move(var_name), std::move(type_node));
        }

        for (auto next = tokens.pop(context);
             next.type != TokenType::CLOSING_PARENTHESIS;
             next = tokens.pop(context)) {
            if (next.type != TokenType::COMMA) {
                context.error(
                    std::format("Expected ')' or ','! Got: {}", next.content));
            }

            auto var_name = [&] {
                utils::TraceBlock block(context, "Parsing argument name");
                return tokens.pop(context, TokenType::IDENTIFIER).content;
            }();

            tokens.pop(context, TokenType::COLON);

            auto type_node = [&] {
                utils::TraceBlock block(context, "Parsing type");
                return parse_type(tokens, context);
            }();

            params.emplace_back(std::move(var_name), std::move(type_node));
        }
    } else {
        tokens.pop(context);
    }

    tokens.pop(context, TokenType::COLON);

    ASTNodePtr nested_value;
    {
        utils::TraceBlock block(context, "Parsing nested expression of lambda");
        nested_value = parse_node(tokens, context);
    }

    return std::make_unique<LambdaNode>(move(params), move(nested_value));
}

static void parse_sequence(
    TokenStream& tokens,
    SyntaxAnalyzerContext& context,
    TokenType terminal_token,
    const function<void(void)>& func)
{
    utils::TraceBlock block(context, "Parsing sequence");
    int num_argument = 1;
    while (tokens.peek(context).type != terminal_token) {
        {
            utils::TraceBlock block(
                context,
                "Parsing " + to_string(num_argument) + ". argument");
            func();
        }
        {
            utils::TraceBlock block(
                context,
                "Parsing token after " + to_string(num_argument) +
                    ". argument");
            TokenType next_type = tokens.peek(context).type;
            if (next_type == terminal_token) {
                return;
            } else if (next_type == TokenType::COMMA) {
                tokens.pop(context);
                if (tokens.peek(context).type == terminal_token) {
                    context.error("Trailing commas are forbidden.");
                }
            } else {
                context.error(
                    "Read unexpected token type '" +
                    token_type_name(next_type) + "'. Expected token types '" +
                    token_type_name(terminal_token) + "' or '" +
                    token_type_name(TokenType::COMMA));
            }
        }
        num_argument++;
    }
}

static ASTNodePtr
parse_function(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    int initial_token_stream_index = tokens.get_position();
    utils::TraceBlock block(context, "Parsing function call");
    Token callee_token = [&] {
        utils::TraceBlock block(context, "Parsing callee literal");
        return tokens.pop(context, TokenType::IDENTIFIER);
    }();
    tokens.pop(context, TokenType::OPENING_PARENTHESIS);
    vector<ASTNodePtr> positional_arguments;
    unordered_map<string, ASTNodePtr> keyword_arguments;
    {
        utils::TraceBlock block(context, "Parsing plugin arguments");
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
        std::make_unique<LiteralNode>(callee_token),
        move(positional_arguments),
        move(keyword_arguments),
        unparsed_config);
}

static const unordered_set<TokenType> literal_tokens{
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
    if (!literal_tokens.contains(token.type)) {
        ostringstream message;
        message << "Token " << token << " cannot be parsed as literal";
        context.error(message.str());
    }
    return std::make_unique<LiteralNode>(token);
}

static ASTNodePtr
parse_list(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Parsing List");
    tokens.pop(context, TokenType::OPENING_BRACKET);
    vector<ASTNodePtr> elements;
    {
        utils::TraceBlock block(context, "Parsing list arguments");
        auto callback = [&]() -> void {
            elements.push_back(parse_node(tokens, context));
        };
        parse_sequence(tokens, context, TokenType::CLOSING_BRACKET, callback);
    }
    tokens.pop(context, TokenType::CLOSING_BRACKET);
    return std::make_unique<ListNode>(move(elements));
}

static const vector<TokenType> parse_node_token_types = {
    TokenType::OPENING_BRACKET,
    TokenType::LET,
    TokenType::LAMBDA,
    TokenType::BOOLEAN,
    TokenType::STRING,
    TokenType::INTEGER,
    TokenType::FLOAT,
    TokenType::DURATION,
    TokenType::IDENTIFIER};

static ASTNodePtr
parse_node(TokenStream& tokens, SyntaxAnalyzerContext& context)
{
    utils::TraceBlock block(context, "Identify node type");
    const Token token = tokens.peek(context);
    if (!ranges::contains(parse_node_token_types, token.type)) {
        context.error(
            "Unexpected token '{}'. "
            "Expected any of the following token types: {}",
            token,
            parse_node_token_types);
    }

    switch (token.type) {
    case TokenType::OPENING_BRACKET: return parse_list(tokens, context);
    case TokenType::LET: return parse_let(tokens, context);
    case TokenType::LAMBDA: return parse_lambda(tokens, context);
    case TokenType::BOOLEAN:
    case TokenType::STRING:
    case TokenType::INTEGER:
    case TokenType::FLOAT:
    case TokenType::DURATION: return parse_literal(tokens, context);
    case TokenType::IDENTIFIER:
        if (tokens.has_tokens(2) &&
            tokens.peek(context, 1).type == TokenType::OPENING_PARENTHESIS) {
            return parse_function(tokens, context);
        } else {
            return parse_literal(tokens, context);
        }
    default:
        throw utils::CriticalError(
            "Unknown token type '{}'.",
            token_type_name(token.type));
    }
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