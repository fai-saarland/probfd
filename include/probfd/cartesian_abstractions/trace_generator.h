#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include <compare>
#include <deque>
#include <format>
#include <memory>
#include <ostream>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

namespace probfd::cartesian_abstractions {
class CartesianAbstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

struct TransitionOutcome {
    int op_id;
    int eff_id;
    int target_id;

    friend auto
    operator<=>(const TransitionOutcome&, const TransitionOutcome&) = default;
};

class TraceGenerator {
public:
    virtual ~TraceGenerator() = default;

    virtual std::unique_ptr<Trace> find_trace(
        CartesianAbstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        downward::utils::CountdownTimer& timer) = 0;

    virtual void notify_split() = 0;
};

} // namespace probfd::cartesian_abstractions

template <>
struct std::formatter<probfd::cartesian_abstractions::TransitionOutcome> {
    std::formatter<std::tuple<int, int, int>> underlying_;

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class FmtContext>
    typename FmtContext::iterator format(
        const probfd::cartesian_abstractions::TransitionOutcome& t,
        FmtContext& ctx) const
    {
        return underlying_.format(
            std::make_tuple(t.op_id, t.eff_id, t.target_id),
            ctx);
    }
};

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H
