#include "language/lexical_analyzer.h"

#include "language/context.h"
#include "language/token.h"
#include "language/token_stream.h"

#include <ranges>
#include <regex>
#include <sstream>

// The MSVC standard library defines this...
#undef IN
#undef TRUE
#undef FALSE

using namespace std;

namespace language::parser {
static regex build_regex(const string& s)
{
    return regex(
        "^\\s*(" + s + ")\\s*",
        regex_constants::ECMAScript | regex_constants::icase);
}

static vector<pair<TokenType, regex>> construct_token_type_expressions()
{
    vector<pair<TokenType, string>> token_type_str_expression = {
        {TokenType::OPENING_PARENTHESIS, R"(\()"},
        {TokenType::CLOSING_PARENTHESIS, R"(\))"},
        {TokenType::OPENING_BRACKET, R"(\[)"},
        {TokenType::CLOSING_BRACKET, R"(\])"},
        {TokenType::COMMA, R"(,)"},
        {TokenType::DOT, R"(\.)"},
        {TokenType::COLON, R"(:)"},
        {TokenType::EQUALS, R"(=)"},
        {TokenType::PLUS, R"(\+)"},
        {TokenType::MINUS, R"(-)"},
        {TokenType::LET, R"(let)"},
        {TokenType::LAMBDA, R"(fun)"},
        {TokenType::TYPE_BOOL, R"(bool)"},
        {TokenType::TYPE_STRING, R"(string)"},
        {TokenType::TYPE_INTEGER, R"(int)"},
        {TokenType::TYPE_FLOAT, R"(float)"},
        {TokenType::TRUE, R"(true)"},
        {TokenType::FALSE, R"(false)"},
        {TokenType::STRING, R"("(\\\\|\\"|\\n|[^"\\])*")"},
        /*
         * Integers and floats may have a user-defined literal suffix, e.g.
         * '4k' or 4.e-2k.
         * this will be translated to a call to __operator_int_k__(4) or
         * __operator_float_k__(4.e-2).
         */
        {TokenType::FLOAT,
         R"((((\d*[.]\d+|\d+[.])(e[+-]?\d+)?)|\d+e[+-]?\d+)([A-Za-z_][A-Za-z_0-9]*)?)"},
        {TokenType::INTEGER, R"([1-9]\d*([A-Za-z_][A-Za-z_0-9]*)?)"},
        {TokenType::IN, R"(in\b)"},
        {TokenType::AS, R"(as\b)"},
        /*
          Identifiers have to be parsed last to prevent reserved words (
          'true', 'false', and 'let') from being recognized as
          identifiers.
        */
        {TokenType::IDENTIFIER, R"([a-zA-Z_0-9]\w*)"}};
    vector<pair<TokenType, regex>> token_type_expression;
    token_type_expression.reserve(token_type_str_expression.size());
    for (const auto& [token_type, regex_str] : token_type_str_expression) {
        token_type_expression.emplace_back(token_type, build_regex(regex_str));
    }
    return token_type_expression;
}

static const vector<pair<TokenType, regex>> token_type_expressions =
    construct_token_type_expressions();

static string highlight_position(const string& text, string::const_iterator pos)
{
    ostringstream message_stream;
    int distance_to_highlight = pos - text.begin();
    for (const auto& line : views::split(text, "\n")) {
        int line_length = line.size();
        bool highlight_in_line =
            distance_to_highlight < line_length && distance_to_highlight >= 0;
        message_stream << (highlight_in_line ? "> " : "  ")
                       << std::string_view(line) << endl;
        if (highlight_in_line) {
            message_stream << string(distance_to_highlight + 2, ' ') << "^"
                           << endl;
        }
        distance_to_highlight -= line.size() + 1;
    }
    string message = message_stream.str();
    message.erase(
        std::ranges::find_if(
            message | std::views::reverse,
            [](int ch) { return !isspace(ch); })
            .base(),
        message.end());
    return message;
}

TokenStream split_tokens(const string& text)
{
    Context context;
    TraceBlock block(context, "Splitting Tokens.");

    vector<Token> tokens;
    auto start = text.begin();
    const auto end = text.end();

    while (start != end) {
        for (const auto& [token_type, expression] : token_type_expressions) {
            if (smatch match; regex_search(
                    start,
                    end,
                    match,
                    expression,
                    regex_constants::match_continuous)) {
                tokens.push_back({match[1], token_type});
                start += match[0].length();
                goto continue_outer;
            }
        }

        context.error(
            "Unable to recognize next token:\n" +
            highlight_position(text, start));

    continue_outer:;
    }
    return TokenStream(move(tokens));
}
} // namespace language::parser