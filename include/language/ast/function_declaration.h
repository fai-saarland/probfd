#ifndef LANGUAGE_TYPED_AST_FUNCTION_DECLARATION_H
#define LANGUAGE_TYPED_AST_FUNCTION_DECLARATION_H

#include "language/ast/namespace_level_declaration.h"

#include <format>
#include <string>
#include <vector>

namespace language::parser {

struct ArgumentInfo {
    std::string key;
    std::string default_value;

private:
    ArgumentInfo(const std::string& key, const std::string& default_value);

public:
    static ArgumentInfo make_optional(const std::string& default_value);
    static ArgumentInfo make_required();

    static ArgumentInfo make_named_optional(
        const std::string& key,
        const std::string& default_value);

    static ArgumentInfo make_named_required(const std::string& key);

    bool has_default() const;

    void set_key(const std::string& key);
    void set_default(const std::string& default_value);

    friend bool operator==(const ArgumentInfo&, const ArgumentInfo&) = default;
};

} // namespace language::parser

template <>
struct std::formatter<language::parser::ArgumentInfo> {
    bool with_default = false;

    // bool with_type = false;

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto pos = ctx.begin();

        for (; pos != ctx.end() && *pos != '}'; ++pos) {
            switch (*pos) {
            case '}': return pos;
            case 'd': {
                if (with_default) {
                    throw std::format_error(
                        std::format("Repeated format specifier: '{}'", *pos));
                }
                with_default = true;
            } break;
                /*case 't': {
                    if (with_type) {
                        throw std::format_error(
                            std::format("Repeated format specifier: '{}'",
                *pos));
                    }
                    with_type = true;
                } break;*/
            default:
                throw std::format_error(
                    std::format("Illegal format specifier: '{}'", *pos));
            }
        }

        return pos;
    }

    template <typename FormatContext>
    auto
    format(const language::parser::ArgumentInfo& info, FormatContext& ctx) const
    {
        auto it = std::format_to(ctx.out(), "{}", info.key);

        // if (with_type) { it = std::format_to(it, " : {}", info.type.name());
        // }

        if (with_default) {
            it = std::format_to(it, " = {}", info.default_value);
        }

        return it;
    }
};

#endif
