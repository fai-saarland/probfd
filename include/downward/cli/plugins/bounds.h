#ifndef PLUGINS_BOUNDS_H
#define PLUGINS_BOUNDS_H

#include <format>
#include <ostream>
#include <string>

namespace downward::cli::plugins {
struct Bounds {
    std::string min;
    std::string max;

public:
    Bounds(const std::string& min, const std::string& max);
    ~Bounds() = default;

    bool has_bound() const;
    static Bounds unlimited();

    friend std::ostream& operator<<(std::ostream& out, const Bounds& bounds);
};
} // namespace downward::cli::plugins

template <>
struct std::formatter<downward::cli::plugins::Bounds> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto
    format(const downward::cli::plugins::Bounds& bounds, FormatContext& ctx)
        const
    {
        return std::format_to(ctx.out(), "[{}, {}]", bounds.min, bounds.max);
    }
};

#endif
