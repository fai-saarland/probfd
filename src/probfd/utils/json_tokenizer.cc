#include "probfd/utils/json_tokenizer.h"

#include <cmath>
#include <concepts>
#include <iostream>
#include <map>
#include <ranges>
#include <vector>

namespace probfd::json {

std::vector<Token> tokenize(std::istream& is)
{
    std::vector<Token> tokens;

    int line = 1;
    int column = 1;

    while (!is.eof()) {
        int next = is.get();

        if (next == '\n') {
            ++line;
            column = 1;
            continue;
        }

        if (std::isspace(next)) {
            ++column;
            continue;
        }

        if (next == EOF) break;

        TokenType type;
        std::string text{static_cast<char>(next)};

        if (next == '-') {
            int c = is.get();

            if (std::isdigit(c)) {
                text.push_back(c);
                goto parse_number;
            }

            /*
            if (is.peek() != 'i') {
                text.push_back(c);
                goto parse_infinity;
            }

            if (is.peek() == 'N') {
                text.push_back(c);
                goto parse_nan;
            }
            */

            throw std::invalid_argument("Expected digit, inf, or NaN.");
        }

        if (isdigit(next)) {
        parse_number:
            type = TokenType::INTEGER;

            for (char c = is.peek(); std::isdigit(c); c = is.peek()) {
                text.push_back(c);
                is.ignore();
            }

            if (int c = is.peek(); c == '.') {
                text.push_back(c);
                is.ignore();
                type = TokenType::FLOAT;

                c = is.get();

                if (!std::isdigit(c)) {
                    throw std::invalid_argument("Expected digit.");
                }

                text.push_back(c);

                for (char c = is.peek(); std::isdigit(c); c = is.peek()) {
                    text.push_back(c);
                    is.ignore();
                }
            }

            if (int c = is.peek(); c == 'e' || c == 'E') {
                text.push_back(c);
                is.ignore();
                type = TokenType::FLOAT;

                c = is.get();

                if (c == '+' || c == '-') {
                    text.push_back(c);
                    c = is.get();
                }

                if (!std::isdigit(c)) {
                    throw std::invalid_argument("Expected digit.");
                }

                text.push_back(c);

                for (char c = is.peek(); std::isdigit(c); c = is.peek()) {
                    text.push_back(c);
                    is.ignore();
                }
            }
        } else if (next == '{') {
            type = TokenType::LEFT_BRACE;
        } else if (next == '}') {
            type = TokenType::RIGHT_BRACE;
        } else if (next == '[') {
            type = TokenType::LEFT_BRACKET;
        } else if (next == ']') {
            type = TokenType::RIGHT_BRACKET;
        } else if (next == ',') {
            type = TokenType::COMMA;
        } else if (next == ':') {
            type = TokenType::COLON;
        } else if (next == 'n') {
            type = TokenType::NUL;
            if (is.get() != 'u' || is.get() != 'l' || is.get() != 'l') {
                throw std::invalid_argument("Expected 'null'.");
            }
        } /* else if (next == 'N') {
        parse_nan:
            type = TokenType::NOT_A_NUMBER;
            if (is.get() != 'a' || is.get() != 'N') {
                throw std::invalid_argument("Expected 'NaN'.");
            }
        } */
        else if (next == 't') {
            type = TokenType::TRUE;
            if (is.get() != 'r' || is.get() != 'u' || is.get() != 'e') {
                throw std::invalid_argument("Expected 'true'.");
            }
        } else if (next == 'f') {
            type = TokenType::FALSE;
            if (is.get() != 'a' || is.get() != 'l' || is.get() != 's' ||
                is.get() != 'e') {
                throw std::invalid_argument("Expected 'false'.");
            }
        } /* else if (next == 'i') {
        parse_infinity:
            type = TokenType::INFTY;
            if (is.get() != 'n' || is.get() != 'f') {
                throw std::invalid_argument("Expected 'inf'.");
            }
        } */
        else if (next == '"') {
            type = TokenType::STRING;

            int c;

            for (c = is.peek(); c != '"'; c = is.peek()) {
                if (c == EOF) {
                    throw std::invalid_argument("Unterminated string.");
                }

                text.push_back(c);
                is.ignore();

                if (c == '\\') {
                    c = is.get();
                    text.push_back(c);

                    if (c == 'u') {

                    } else if (
                        c != '"' && c != '\\' && c != '/' && c != 'b' &&
                        c != 'f' && c != 'n' && c != 'r' && c != 't') {
                        throw std::invalid_argument(
                            "Expected valid escaped symbol.");
                    }
                }
            }

            text.push_back(c);
            is.ignore();
        } else {
            throw std::invalid_argument("Invalid character.");
        }

        const int text_length = text.length();
        tokens.emplace_back(type, std::move(text), line, column);
        column += text_length;
    }

    tokens.emplace_back(TokenType::END_OF_FILE, std::move(""), line, column);

    return tokens;
}

} // namespace probfd::json
