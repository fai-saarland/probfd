#ifndef DOWNWARD_LANDMARKS_LANDMARK_COST_PARTITIONING_HEURISTIC_H
#define DOWNWARD_LANDMARKS_LANDMARK_COST_PARTITIONING_HEURISTIC_H

#include "downward/landmarks/landmark_heuristic.h"

#include "downward/lp/lp_solver.h"

namespace downward::landmarks {
class CostPartitioningAlgorithm;

enum class CostPartitioningMethod {
    OPTIMAL,
    UNIFORM,
};

class LandmarkCostPartitioningHeuristic : public LandmarkHeuristic {
    std::unique_ptr<CostPartitioningAlgorithm> cost_partitioning_algorithm;

    void check_unsupported_features(
        const std::shared_ptr<LandmarkFactory>& lm_factory);
    void set_cost_partitioning_algorithm(
        CostPartitioningMethod cost_partitioning,
        lp::LPSolverType lpsolver,
        bool alm);

    int get_heuristic_value(const State& ancestor_state) override;

public:
    LandmarkCostPartitioningHeuristic(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        bool pref,
        bool prog_goal,
        bool prog_gn,
        bool prog_r,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity,
        CostPartitioningMethod cost_partitioning,
        bool alm,
        lp::LPSolverType lpsolver);

    LandmarkCostPartitioningHeuristic(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        bool pref,
        bool prog_goal,
        bool prog_gn,
        bool prog_r,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity,
        CostPartitioningMethod cost_partitioning,
        bool alm,
        lp::LPSolverType lpsolver);

    ~LandmarkCostPartitioningHeuristic() override;

    virtual bool dead_ends_are_reliable() const override;
};
} // namespace landmarks

#endif // DOWNWARD_LANDMARKS_LANDMARK_COST_PARTITIONING_HEURISTIC_H
