#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_ASTAR_TRACE_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_ASTAR_TRACE_GENERATOR_H

#include "probfd/cartesian_abstractions/trace_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include "downward/algorithms/priority_queues.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

namespace probfd::cartesian_abstractions {
class CartesianAbstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {
/*
  Find an optimal trace in the determinization using A*.
*/
class AStarTraceGenerator : public TraceGenerator {
    class AbstractSearchInfo;

    // Keep data structures around to avoid reallocating them.
    priority_queues::HeapQueue<value_t, int> open_queue_;
    std::vector<AbstractSearchInfo> search_info_;

    std::unique_ptr<Trace>
    extract_solution(int init_id, int goal_id, utils::CountdownTimer& timer)
        const;

    void update_heuristic(
        CartesianAbstraction& abstraction,
        CartesianHeuristic& heuristic,
        const Trace& solution) const;

public:
    AStarTraceGenerator();

    std::unique_ptr<Trace> find_trace(
        CartesianAbstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer) override;

    void notify_split() override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_ASTAR_TRACE_GENERATOR_H
