#ifndef LANGUAGE_AST_QUALIFIED_NAME_H
#define LANGUAGE_AST_QUALIFIED_NAME_H

#include <format>
#include <string>
#include <vector>

namespace language::parser {

struct QualifiedName {
    std::vector<std::string> qualification_prefix;
    std::string name;
};

} // namespace language::parser

// Formatter specialization for QualifiedName
template <>
struct std::formatter<language::parser::QualifiedName> {
    std::range_formatter<std::string> fmt;

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx);

    template <typename FormatContext>
    typename FormatContext::iterator
    format(const language::parser::QualifiedName& name, FormatContext& ctx)
        const;
};

template <typename ParseContext>
constexpr typename ParseContext::iterator
std::formatter<language::parser::QualifiedName>::parse(ParseContext& ctx)
{
    fmt.set_separator(".");
    fmt.set_brackets("", "");
    return ctx.begin();
}

template <typename FormatContext>
typename FormatContext::iterator
std::formatter<language::parser::QualifiedName>::format(
    const language::parser::QualifiedName& name,
    FormatContext& ctx) const
{
    if (!name.qualification_prefix.empty()) {
        auto it = fmt.format(name.qualification_prefix, ctx);
        ctx.advance_to(it);
    }

    return std::format_to(ctx.out(), "{}", name.name);
}

#endif
