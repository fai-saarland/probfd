#include "probfd/json/tokenize.h"

#include <concepts>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <vector>

namespace probfd::json {

namespace {

template <typename...>
struct AllSameHelper : std::true_type {};

template <typename H, typename... T>
struct AllSameHelper<H, T...>
    : std::bool_constant<(std::is_same_v<H, T> && ...)> {};

template <typename... T>
concept AllSame = AllSameHelper<T...>::value;

template <typename... Ts>
    requires AllSame<std::decay_t<Ts>...>
constexpr auto concat_strings(Ts&&... args)
{
    using charType =
        std::remove_const_t<std::remove_pointer_t<std::common_type_t<Ts...>>>;

    constexpr size_t charCount =
        ((sizeof(Ts) + ...) - sizeof...(Ts) + sizeof(charType)) /
        sizeof(charType);

    std::array<charType, charCount> result{};
    result[charCount - 1] = '\0';

    size_t pos = 0;
    auto detail_concat = [&pos, &result](auto&& arg) {
        constexpr auto count =
            (sizeof(arg) - sizeof(charType)) / sizeof(charType);

        for (size_t i = 0; i < count; ++i) { result[pos++] = arg[i]; }
    };

    (detail_concat(args), ...);

    return result;
}

template <std::size_t N>
struct ConstString {
    char str[N]{};

    static constexpr std::size_t size = N - 1;

    [[nodiscard]]
    constexpr const auto& view() const
    {
        return str;
    }

    consteval ConstString() {}

    consteval ConstString(const char (&new_str)[N])
    {
        if (new_str[N - 1] != '\0') abort();
        std::copy_n(new_str, size, str);
    }
};

template <ConstString format_string, typename... Args>
[[noreturn]]
void throw_error(std::size_t line, std::size_t column, Args&&... args)
{
    static constexpr std::array format =
        concat_strings("Error at line {}, column {}: ", format_string.view());

    throw TokenizationError(
        std::string_view(format.data(), format.size()),
        line,
        column,
        std::forward<Args>(args)...);
}
} // namespace

std::vector<Token> tokenize(std::istream& is)
{
    std::vector<Token> tokens;

    std::size_t line = 1;
    std::size_t column = 1;

    while (!is.eof()) {
        const int next = is.get();

        if (next == '\n') {
            ++line;
            column = 1;
            continue;
        }

        if (next == '\t') {
            column += 4;
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
            if (const int c = is.get(); std::isdigit(c)) {
                text.push_back(static_cast<char>(c));
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

            throw_error<"Expected digit, inf, or NaN.">(line, column);
        }

        if (isdigit(next)) {
        parse_number:
            type = TokenType::INTEGER;

            for (int c = is.peek(); std::isdigit(c); c = is.peek()) {
                text.push_back(static_cast<char>(c));
                is.ignore();
            }

            if (int c = is.peek(); c == '.') {
                text.push_back(static_cast<char>(c));
                is.ignore();
                type = TokenType::FLOAT;

                c = is.get();

                if (!std::isdigit(c)) {
                    throw_error<"Expected digit.">(line, column);
                }

                text.push_back(static_cast<char>(c));

                for (c = is.peek(); std::isdigit(c); c = is.peek()) {
                    text.push_back(static_cast<char>(c));
                    is.ignore();
                }
            }

            if (int c = is.peek(); c == 'e' || c == 'E') {
                text.push_back(static_cast<char>(c));
                is.ignore();
                type = TokenType::FLOAT;

                c = is.get();

                if (c == '+' || c == '-') {
                    text.push_back(static_cast<char>(c));
                    c = is.get();
                }

                if (!std::isdigit(c)) {
                    throw_error<"Expected digit.">(line, column);
                }

                text.push_back(static_cast<char>(c));

                for (c = is.peek(); std::isdigit(c); c = is.peek()) {
                    text.push_back(static_cast<char>(c));
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
                throw_error<"Unknown token.">(line, column);
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
                throw_error<"Unknown token.">(line, column);
            }
        } else if (next == 'f') {
            type = TokenType::FALSE;
            if (is.get() != 'a' || is.get() != 'l' || is.get() != 's' ||
                is.get() != 'e') {
                throw_error<"Unknown token.">(line, column);
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
                    throw_error<"Unterminated string.">(line, column);
                }

                text.push_back(static_cast<char>(c));
                is.ignore();

                if (c == '\\') {
                    c = is.get();
                    text.push_back(static_cast<char>(c));

                    if (c == 'u') {

                    } else if (
                        c != '"' && c != '\\' && c != '/' && c != 'b' &&
                        c != 'f' && c != 'n' && c != 'r' && c != 't') {
                        throw_error<"Invalid escaped symbol.">(
                            line,
                            column + text.size());
                    }
                }
            }

            text.push_back(static_cast<char>(c));
            is.ignore();
        } else {
            throw_error<"Invalid character.">(line, column);
        }

        const std::size_t text_length = text.length();
        tokens.emplace_back(type, std::move(text), line, column);
        column += text_length;
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);

    return tokens;
}

} // namespace probfd::json
