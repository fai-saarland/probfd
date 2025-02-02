#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H

#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include <compare>
#include <deque>
#include <memory>
#include <ostream>

// Forward Declarations
namespace utils {
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

    friend std::ostream&
    operator<<(std::ostream& os, const TransitionOutcome& t);
};

class TraceGenerator {
public:
    virtual ~TraceGenerator() = default;

    virtual std::unique_ptr<Trace> find_trace(
        CartesianAbstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer) = 0;

    virtual void notify_split() = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_TRACE_GENERATOR_H
