#ifndef LANDMARKS_LANDMARK_HEURISTIC_H
#define LANDMARKS_LANDMARK_HEURISTIC_H

#include "downward/heuristic.h"

#include "downward/utils/hash.h"

namespace downward {
class ConstBitsetView;
}

namespace downward::successor_generator {
class SuccessorGenerator;
}

namespace downward::landmarks {
class LandmarkFactory;
class LandmarkGraph;
class LandmarkNode;
class LandmarkStatusManager;

class LandmarkHeuristic : public Heuristic {
    bool initial_landmark_graph_has_cycle_of_natural_orderings;

    /* TODO: We would prefer the following two functions to be implemented
        somewhere else as more generic graph algorithms. */
    bool landmark_graph_has_cycle_of_natural_orderings();
    bool depth_first_search_for_cycle_of_natural_orderings(
        const LandmarkNode& node,
        std::vector<bool>& closed,
        std::vector<bool>& visited);

protected:
    std::shared_ptr<LandmarkGraph> lm_graph;
    const bool use_preferred_operators;
    // This map remains empty unless *use_preferred_operators* is true.
    utils::HashMap<FactPair, std::unordered_set<int>>
        landmarks_achieved_by_fact;

    std::unique_ptr<LandmarkStatusManager> lm_status_manager;
    std::unique_ptr<successor_generator::SuccessorGenerator>
        successor_generator;

    void initialize(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        bool prog_goal,
        bool prog_gn,
        bool prog_r);
    void
    compute_landmark_graph(const std::shared_ptr<LandmarkFactory>& lm_factory);

    virtual int get_heuristic_value(const State& ancestor_state) = 0;

    bool operator_is_preferred(
        const OperatorProxy &op, const State &state, ConstBitsetView &future);
    void compute_landmarks_achieved_by_fact();
    void
    generate_preferred_operators(const State& state, ConstBitsetView& future);
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    LandmarkHeuristic(
        bool use_preferred_operators,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    LandmarkHeuristic(
        bool use_preferred_operators,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~LandmarkHeuristic() override;

    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override
    {
        evals.insert(this);
    }

    virtual void notify_initial_state(const State& initial_state) override;
    virtual void notify_state_transition(
        const State& parent_state,
        OperatorID op_id,
        const State& state) override;
};

} // namespace landmarks

#endif
