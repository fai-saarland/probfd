#ifndef LANGUAGE_AST_QUALIFIED_NAME_H
#define LANGUAGE_AST_QUALIFIED_NAME_H

#include "downward/utils/strings.h"

#include <format>
#include <string>
#include <vector>

namespace downward::cli::parser {

struct QualifiedName {
    std::vector<std::string> qualification_prefix;
    std::string name;
};

} // namespace downward::cli::parser

// Formatter specialization for QualifiedName
template <>
struct std::formatter<downward::cli::parser::QualifiedName> {
    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx);

    template <typename FormatContext>
    typename FormatContext::iterator
    format(const downward::cli::parser::QualifiedName& name, FormatContext& ctx)
        const;
};

template <typename ParseContext>
constexpr typename ParseContext::iterator
std::formatter<downward::cli::parser::QualifiedName, char>::parse(
    ParseContext& ctx)
{
    return ctx.begin();
}

template <typename FormatContext>
typename FormatContext::iterator
std::formatter<downward::cli::parser::QualifiedName, char>::format(
    const downward::cli::parser::QualifiedName& name,
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
