#ifndef HEURISTICS_MAX_HEURISTIC_H
#define HEURISTICS_MAX_HEURISTIC_H

#include "downward/heuristics/relaxation_heuristic.h"

#include "downward/algorithms/priority_queues.h"

#include <cassert>

namespace downward::max_heuristic {
using relaxation_heuristic::OpID;
using relaxation_heuristic::PropID;

using relaxation_heuristic::Proposition;
using relaxation_heuristic::UnaryOperator;

class HSPMaxHeuristic : public relaxation_heuristic::RelaxationHeuristic {
    priority_queues::AdaptiveQueue<PropID> queue;

    void setup_exploration_queue();
    void setup_exploration_queue_state(const State& state);
    void relaxed_exploration();

    void enqueue_if_necessary(PropID prop_id, int cost)
    {
        assert(cost >= 0);
        Proposition* prop = get_proposition(prop_id);
        if (prop->cost == -1 || prop->cost > cost) {
            prop->cost = cost;
            queue.push(cost, prop_id);
        }
        assert(prop->cost != -1 && prop->cost <= cost);
    }

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    HSPMaxHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    HSPMaxHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace max_heuristic

#endif
