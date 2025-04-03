
#include "probfd/json/json.h"

#include <cassert>
#include <format>
#include <iostream>
#include <map>
#include <ranges>
#include <vector>

namespace probfd::json {

JsonObject::JsonObject(
    std::map<std::string, std::unique_ptr<JsonElement>> members)
    : JsonElement(ElementID::OBJECT)
    , members(std::move(members))
{
}

void JsonObject::print(std::ostream& os, int indent)
{
    if (members.empty()) {
        std::print(os, "{{}}");
        return;
    }

    std::println(os, "{{");

    auto it = members.begin();
    const auto end = members.end();

    {
        const auto& [name, element] = *it;
        std::print(os, "{:>{}}\"{}\": ", ' ', indent + 4, name);
        element->print(os, indent + 4);
    }

    for (++it; it != end; ++it) {
        const auto& [name, element] = *it;
        std::println(os, ",");
        std::print(os, "{:>{}}\"{}\": ", ' ', indent + 4, name);
        element->print(os, indent + 4);
    }

    std::println(os);

    if (indent == 0) {
        std::print(os, "}}");
    } else {
        std::print(os, "{:>{}}}}", ' ', indent);
    }
}

const JsonElement* JsonObject::get_field(const std::string& s) const
{
    const auto it = members.find(s);
    if (it == members.end()) return nullptr;
    return it->second.get();
}

JsonArray::JsonArray(std::vector<std::unique_ptr<JsonElement>> elements)
    : JsonElement(ElementID::ARRAY)
    , elements(std::move(elements))
{
}

void JsonArray::print(std::ostream& os, int indent)
{
    if (elements.empty()) {
        std::print(os, "[]");
        return;
    }

    std::println(os, "[");
    std::print(os, "{:>{}}", ' ', indent + 4);
    elements.front()->print(os, indent + 4);

    for (const auto& element : elements | std::views::drop(1)) {
        std::println(os, ",");
        std::print(os, "{:>{}}", ' ', indent + 4);
        element->print(os, indent + 4);
    }

    std::println(os);

    if (indent == 0) {
        std::print(os, "]");
    } else {
        std::print(os, "{:>{}}]", ' ', indent);
    }
}

void JsonString::print(std::ostream& os, int)
{
    std::print(os, "{}", string);
}

void JsonInteger::print(std::ostream& os, int)
{
    std::print(os, "{}", value);
}

void JsonFloat::print(std::ostream& os, int)
{
    if (value == std::numeric_limits<double>::infinity()) {
        std::print(os, "\"inf\"");
    } else if (value == -std::numeric_limits<double>::infinity()) {
        std::print(os, "\"-inf\"");
    } else if (value == std::numeric_limits<double>::quiet_NaN()) {
        std::print(os, "\"NaN\"");
    } else if (value == -std::numeric_limits<double>::quiet_NaN()) {
        std::print(os, "\"-NaN\"");
    } else {
        std::print(os, "{}", value);
    }
}

void JsonBoolean::print(std::ostream& os, int)
{
    if (value) {
        std::print(os, "true");
    } else {
        std::print(os, "false");
    }
}

void JsonNull::print(std::ostream& os, int)
{
    std::print(os, "null");
}

std::unique_ptr<JsonElement> parse_element(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it)
{
    assert(it != tokens.end());

    if (it->type == TokenType::END_OF_FILE) {
        throw std::invalid_argument(
            "Unexpected end of file while parsing element.");
    }

    if (it->type == TokenType::LEFT_BRACKET) {
        return parse_array(tokens, it);
    }

    if (it->type == TokenType::LEFT_BRACE) {
        return parse_object(tokens, it);
    }

    if (it->type == TokenType::STRING) {
        auto r = std::make_unique<JsonString>(
            it->text.substr(1, it->text.length() - 2));
        ++it;
        return r;
    }

    if (it->type == TokenType::INTEGER) {
        return std::make_unique<JsonInteger>(std::stoll(it++->text));
    }

    if (it->type == TokenType::FLOAT) {
        return std::make_unique<JsonFloat>(std::stold(it++->text));
    }

    if (it->type == TokenType::NUL) {
        ++it;
        return std::make_unique<JsonNull>();
    }

    if (it->type == TokenType::TRUE) {
        ++it;
        return std::make_unique<JsonBoolean>(true);
    }

    if (it->type == TokenType::FALSE) {
        ++it;
        return std::make_unique<JsonBoolean>(false);
    }

    throw std::invalid_argument("Expected element");
}

std::unique_ptr<JsonObject> parse_object(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it)
{
    assert(it != tokens.end());

    if (it->type == TokenType::END_OF_FILE) {
        throw std::invalid_argument(
            "Unexpected end of file while parsing object.");
    }

    if (it->type != TokenType::LEFT_BRACE) {
        throw std::invalid_argument("Expected '{'");
    }

    ++it;

    std::map<std::string, std::unique_ptr<JsonElement>> members;

    if (it->type != TokenType::RIGHT_BRACE) {
        for (;;) {
            if (it->type == TokenType::END_OF_FILE) {
                throw std::invalid_argument(
                    "Unexpected end of file while parsing object.");
            }

            if (it->type != TokenType::STRING) {
                throw std::invalid_argument("Expected field");
            }

            std::string field_name = it->text;

            ++it;

            if (it->type == TokenType::END_OF_FILE) {
                throw std::invalid_argument(
                    "Unexpected end of file while parsing object.");
            }

            if (it->type != TokenType::COLON) {
                throw std::invalid_argument("Expected colon");
            }

            ++it;

            std::unique_ptr member = parse_element(tokens, it);

            auto [_, inserted] = members.emplace(
                field_name.substr(1, field_name.length() - 2),
                std::move(member));

            if (!inserted) {
                throw std::invalid_argument(
                    std::format("Found duplicate field: {}", field_name));
            }

            if (it->type == TokenType::END_OF_FILE) {
                throw std::invalid_argument(
                    "Unexpected end of file while parsing object.");
            }

            if (it->type == TokenType::RIGHT_BRACE) break;

            if (it->type != TokenType::COMMA) {
                throw std::invalid_argument("Expected ',' or '}'");
            }

            ++it;
        }
    }

    ++it;

    return std::make_unique<JsonObject>(std::move(members));
}

std::unique_ptr<JsonArray> parse_array(
    const std::vector<Token>& tokens,
    std::vector<Token>::const_iterator& it)
{
    assert(it != tokens.end());

    if (it->type == TokenType::END_OF_FILE) {
        throw std::invalid_argument(
            "Unexpected end of file while parsing object.");
    }

    if (it->type != TokenType::LEFT_BRACKET) {
        throw std::invalid_argument("Expected '['");
    }

    ++it;

    std::vector<std::unique_ptr<JsonElement>> members;

    if (it->type != TokenType::RIGHT_BRACKET) {
        for (;;) {
            std::unique_ptr member = parse_element(tokens, it);
            members.emplace_back(std::move(member));

            if (it->type == TokenType::END_OF_FILE) {
                throw std::invalid_argument(
                    "Unexpected end of file while parsing object.");
            }

            if (it->type == TokenType::RIGHT_BRACKET) break;

            if (it->type != TokenType::COMMA) {
                throw std::invalid_argument("Expected ',' or ']'");
            }

            ++it;
        }
    }

    ++it;

    return std::make_unique<JsonArray>(std::move(members));
}

std::unique_ptr<JsonElement> parse_element(const std::vector<Token>& tokens)
{
    auto it = tokens.begin();
    auto r = parse_element(tokens, it);

    if (it->type != TokenType::END_OF_FILE) {
        throw std::invalid_argument("Extra tokens after parsing array.");
    }

    return r;
}

std::unique_ptr<JsonObject> parse_object(const std::vector<Token>& tokens)
{
    auto it = tokens.begin();
    auto r = parse_object(tokens, it);

    if (it->type != TokenType::END_OF_FILE) {
        throw std::invalid_argument("Extra tokens after parsing array.");
    }

    return r;
}

std::unique_ptr<JsonArray> parse_array(const std::vector<Token>& tokens)
{
    auto it = tokens.begin();
    auto r = parse_array(tokens, it);

    if (it->type != TokenType::END_OF_FILE) {
        throw std::invalid_argument("Extra tokens after parsing array.");
    }

    return r;
}

} // namespace probfd::json
