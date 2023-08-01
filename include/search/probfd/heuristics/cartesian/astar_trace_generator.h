#ifndef PROBFD_HEURISTICS_CARTESIAN_ASTAR_TRACE_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ASTAR_TRACE_GENERATOR_H

#include "probfd/heuristics/cartesian/trace_generator.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

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
        CartesianCostFunction& cost_function,
        CartesianHeuristic& heuristic,
        const Trace& solution) const;

public:
    AStarTraceGenerator();

    std::unique_ptr<Trace> find_trace(
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        int init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& timer) override;

    void notify_split() override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
