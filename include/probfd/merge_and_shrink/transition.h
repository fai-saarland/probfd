#ifndef PROBFD_MERGE_AND_SHRINK_TRANSITION_H
#define PROBFD_MERGE_AND_SHRINK_TRANSITION_H

#include "probfd/utils/format.h"
#include "probfd/utils/json.h"

#include <compare>
#include <format>
#include <iosfwd>
#include <vector>

namespace probfd::merge_and_shrink {

struct Transition {
    int src;
    std::vector<int> targets;

    explicit Transition(const json::JsonObject& object);

    Transition() = default;
    Transition(int src, std::vector<int> targets);

    friend auto
    operator<=>(const Transition& lhs, const Transition& rhs) = default;

    friend std::ostream&
    operator<<(std::ostream& os, const Transition& transition);

    friend std::unique_ptr<json::JsonObject>
    to_json(const Transition& transition);
};

} // namespace probfd::merge_and_shrink

template <>
struct std::formatter<probfd::merge_and_shrink::Transition> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(
        const probfd::merge_and_shrink::Transition& t,
        std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{} -> ({:n})", t.src, t.targets);
    }
};

#endif
