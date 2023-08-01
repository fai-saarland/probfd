#ifndef PROBFD_HEURISTICS_CARTESIAN_TRACE_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_TRACE_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/types.h"

#include <compare>
#include <deque>
#include <memory>
#include <ostream>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace heuristics {
namespace cartesian {

class Abstraction;
class CartesianCostFunction;
class CartesianHeuristic;

struct TransitionOutcome {
    int op_id;
    int eff_id;
    int target_id;

    friend auto
    operator<=>(const TransitionOutcome&, const TransitionOutcome&) = default;

    friend std::ostream&
    operator<<(std::ostream& os, const TransitionOutcome& t);
};

class TraceGenerator {
public:
    virtual ~TraceGenerator() = default;

    virtual std::unique_ptr<Trace> find_trace(
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer) = 0;

    virtual void notify_split() = 0;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
