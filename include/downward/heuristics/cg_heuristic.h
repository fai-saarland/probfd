#ifndef HEURISTICS_CG_HEURISTIC_H
#define HEURISTICS_CG_HEURISTIC_H

#include "downward/heuristic.h"

#include "downward/algorithms/priority_queues.h"

#include <memory>
#include <string>
#include <vector>

namespace downward::domain_transition_graph {
class DomainTransitionGraph;
struct ValueNode;
} // namespace domain_transition_graph

namespace downward::cg_heuristic {
class CGCache;

class CGHeuristic : public Heuristic {
    using ValueNodeQueue =
        priority_queues::AdaptiveQueue<domain_transition_graph::ValueNode*>;
    std::vector<std::unique_ptr<ValueNodeQueue>> prio_queues;
    std::vector<std::unique_ptr<domain_transition_graph::DomainTransitionGraph>>
        transition_graphs;

    std::unique_ptr<CGCache> cache;
    int cache_hits;
    int cache_misses;

    int helpful_transition_extraction_counter;

    int min_action_cost;

    void setup_domain_transition_graphs();
    int get_transition_cost(
        const State& state,
        domain_transition_graph::DomainTransitionGraph* dtg,
        int start_val,
        int goal_val);
    void mark_helpful_transitions(
        const State& state,
        domain_transition_graph::DomainTransitionGraph* dtg,
        int to);

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    CGHeuristic(
        int max_cache_size,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    CGHeuristic(
        int max_cache_size,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~CGHeuristic() override;

    virtual bool dead_ends_are_reliable() const override;
};
} // namespace cg_heuristic

#endif
