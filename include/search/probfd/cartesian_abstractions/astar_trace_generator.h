#ifndef PROBFD_CARTESIAN_ASTAR_TRACE_GENERATOR_H
#define PROBFD_CARTESIAN_ASTAR_TRACE_GENERATOR_H

#include "probfd/cartesian_abstractions/trace_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include "downward/algorithms/priority_queues.h"

#include <memory>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace cartesian_abstractions {
class Abstraction;
class CartesianHeuristic;

/*
  Find an optimal trace in the determinization using A*.
*/
class AStarTraceGenerator : public TraceGenerator {
    class AbstractSearchInfo;

    // Keep data structures around to avoid reallocating them.
    priority_queues::HeapQueue<value_t, int> open_queue;
    std::vector<AbstractSearchInfo> search_info;

    std::unique_ptr<Trace>
    extract_solution(int init_id, int goal_id, utils::CountdownTimer& timer)
        const;

    void update_heuristic(
        Abstraction& abstraction,
        CartesianHeuristic& heuristic,
        const Trace& solution) const;

public:
    AStarTraceGenerator();

    std::unique_ptr<Trace> find_trace(
        Abstraction& abstraction,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer) override;

    void notify_split() override;
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_ASTAR_TRACE_GENERATOR_H
