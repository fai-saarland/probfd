#ifndef LANGUAGE_AST_QUALIFIED_NAME_H
#define LANGUAGE_AST_QUALIFIED_NAME_H

#include "downward/utils/strings.h"

#include <format>
#include <string>
#include <vector>

namespace language::parser {

struct QualifiedName {
    std::vector<std::string> qualification_prefix;
    std::string name;
};

} // namespace downward::cli::parser

// Formatter specialization for QualifiedName
template <>
struct std::formatter<language::parser::QualifiedName> {
    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx);

    template <typename FormatContext>
    typename FormatContext::iterator
    format(const language::parser::QualifiedName& name, FormatContext& ctx)
        const;
};

template <typename ParseContext>
constexpr typename ParseContext::iterator
std::formatter<language::parser::QualifiedName>::parse(
    ParseContext& ctx)
{
    return ctx.begin();
}

template <typename FormatContext>
typename FormatContext::iterator
std::formatter<language::parser::QualifiedName>::format(
    const language::parser::QualifiedName& name,
    FormatContext& ctx) const
{
    if (name.qualification_prefix.empty()) {
        return std::format_to(ctx.out(), "{}", name.name);
    }

    return std::format_to(
        ctx.out(),
        "{}.{}",
        downward::utils::join_view(name.qualification_prefix, "."),
        name.name);
}

#endif
