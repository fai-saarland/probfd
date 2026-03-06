#ifndef LANGUAGE_AST_QUALIFIED_NAME_H
#define LANGUAGE_AST_QUALIFIED_NAME_H

#include <format>
#include <ranges>
#include <string>
#include <vector>

namespace language::parser {

template <std::ranges::input_range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, std::string>
struct NamespaceFormatted {
    const R& range;
};

struct QualifiedName {
    // This always has at least one part, the last part is the name to be looked
    // up in the corresponding scope designated by the previous parts.
    std::vector<std::string> parts;

public:
    bool is_qualified() const { return parts.size() > 1; }

    auto qualification_begin() const { return parts.begin(); }

    auto qualification_end() const { return std::prev(parts.end()); }

    auto qualification_prefix() const
    {
        return std::ranges::subrange(
            qualification_begin(),
            qualification_end());
    }

    const auto& name() const { return parts.back(); }
};

} // namespace language::parser

// Formatter specialization for NamespaceFormatted
template <std::ranges::input_range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, std::string>
struct std::formatter<language::parser::NamespaceFormatted<R>> {
    std::range_formatter<std::string> fmt;

    template <typename ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx);

    template <typename FormatContext>
    typename FormatContext::iterator format(
        const language::parser::NamespaceFormatted<R>& name,
        FormatContext& ctx) const;
};

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

template <std::ranges::input_range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, std::string>
template <typename ParseContext>
constexpr typename ParseContext::iterator
std::formatter<language::parser::NamespaceFormatted<R>>::parse(
    ParseContext& ctx)
{
    fmt.set_separator(".");
    fmt.set_brackets("", "");
    return ctx.begin();
}

template <std::ranges::input_range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, std::string>
template <typename FormatContext>
typename FormatContext::iterator
std::formatter<language::parser::NamespaceFormatted<R>>::format(
    const language::parser::NamespaceFormatted<R>& name,
    FormatContext& ctx) const
{
    if (!name.range.empty()) { return fmt.format(name.range, ctx); }

    return ctx.out();
}

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
    return fmt.format(name.parts, ctx);
}

#endif
