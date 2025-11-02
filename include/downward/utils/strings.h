#ifndef UTILS_STRINGS_H
#define UTILS_STRINGS_H

#include "downward/utils/exceptions.h"

#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace downward::utils {
extern void lstrip(std::string& s);
extern void rstrip(std::string& s);
extern void strip(std::string& s);

/*
  Split a given string at the first max_splits occurrence of separator. Use
  the default of -1 for an unlimited amount of splits.
*/
extern std::vector<std::string>
split(const std::string& s, const std::string& separator, int max_splits = -1);

extern bool startswith(const std::string& s, const std::string& prefix);

extern std::string tolower(std::string s);

extern bool is_alpha_numeric(const std::string& s);

template <typename R, typename Char = char>
struct join_view {
    R range;
    std::basic_string_view<Char> sep;

    join_view(R&& range, std::basic_string_view<Char> s)
        : range(std::forward<R>(range))
        , sep(std::move(s))
    {
    }
};

template <typename R>
join_view(R&& r, std::string c) -> join_view<std::views::all_t<R>>;

} // namespace downward::utils

namespace std {

template <typename R, typename Char>
struct formatter<downward::utils::join_view<R, Char>, Char> {
private:
    using value_type = std::ranges::range_value_t<const R>;
    using It = std::ranges::iterator_t<const R>;

    std::formatter<std::remove_cvref_t<value_type>, Char> value_formatter_;

public:
    constexpr auto parse(std::basic_format_parse_context<Char>& ctx)
    {
        return value_formatter_.parse(ctx);
    }

    template <typename FormatContext>
    auto
    format(const downward::utils::join_view<R, Char>& value, FormatContext& ctx)
        const -> decltype(ctx.out())
    {
        It it = std::ranges::begin(value.range);
        auto out = ctx.out();
        if (it == std::ranges::end(value.range)) return out;
        out = value_formatter_.format(*it, ctx);
        for (++it; it != std::ranges::end(value.range); ++it) {
            out = std::copy(value.sep.begin(), value.sep.end(), out);
            ctx.advance_to(out);
            out = value_formatter_.format(*it, ctx);
        }
        return out;
    }
};

/*
template <typename It, typename Sentinel, typename Char>
constexpr bool
    enable_nonlocking_formatter_optimization<join_view<It, Sentinel, Char>>
= true;
*/

} // namespace std
#endif
