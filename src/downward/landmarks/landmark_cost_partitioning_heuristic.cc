#include "downward/landmarks/landmark_cost_partitioning_heuristic.h"

#include "downward/landmarks/landmark_cost_partitioning_algorithms.h"
#include "downward/landmarks/landmark_factory.h"
#include "downward/landmarks/landmark_status_manager.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"

#include "downward/task_transformation.h"

#include <cmath>
#include <limits>

using namespace std;

namespace downward::landmarks {
LandmarkCostPartitioningHeuristic::LandmarkCostPartitioningHeuristic(
    const shared_ptr<LandmarkFactory>& lm_factory,
    bool pref,
    bool prog_goal,
    bool prog_gn,
    bool prog_r,
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity,
    CostPartitioningMethod cost_partitioning,
    bool alm,
    lp::LPSolverType lpsolver)
    : LandmarkHeuristic(
          pref,
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
{
    if (log.is_at_least_normal()) {
        log << "Initializing landmark cost partitioning heuristic..." << endl;
    }
    check_unsupported_features(lm_factory);
    initialize(lm_factory, prog_goal, prog_gn, prog_r);
    set_cost_partitioning_algorithm(cost_partitioning, lpsolver, alm);
}

LandmarkCostPartitioningHeuristic::LandmarkCostPartitioningHeuristic(
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
    lp::LPSolverType lpsolver)
    : LandmarkCostPartitioningHeuristic(
          lm_factory,
          pref,
          prog_goal,
          prog_gn,
          prog_r,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity,
          cost_partitioning,
          alm,
          lpsolver)
{
}

LandmarkCostPartitioningHeuristic::~LandmarkCostPartitioningHeuristic()
    = default;

void LandmarkCostPartitioningHeuristic::check_unsupported_features(
    const shared_ptr<LandmarkFactory>& lm_factory)
{
    if (task_properties::has_axioms(task_proxy)) {
        cerr << "Cost partitioning does not support axioms." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
    }

    if (!lm_factory->supports_conditional_effects() &&
        task_properties::has_conditional_effects(task_proxy)) {
        cerr << "Conditional effects not supported by the landmark "
             << "generation method." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
    }
}

void LandmarkCostPartitioningHeuristic::set_cost_partitioning_algorithm(
    CostPartitioningMethod cost_partitioning,
    lp::LPSolverType lpsolver,
    bool alm)
{
    if (cost_partitioning == CostPartitioningMethod::OPTIMAL) {
        cost_partitioning_algorithm =
            std::make_unique<OptimalCostPartitioningAlgorithm>(
                task_properties::get_operator_costs(task_proxy),
                *lm_graph,
                lpsolver);
    } else if (cost_partitioning == CostPartitioningMethod::UNIFORM) {
        cost_partitioning_algorithm =
            std::make_unique<UniformCostPartitioningAlgorithm>(
                task_properties::get_operator_costs(task_proxy),
                *lm_graph,
                alm);
    } else {
        ABORT("Unknown cost partitioning method");
    }
}

int LandmarkCostPartitioningHeuristic::get_heuristic_value(
    const State& ancestor_state)
{
    double epsilon = 0.01;

    double h_val =
        cost_partitioning_algorithm->get_cost_partitioned_heuristic_value(
            *lm_status_manager,
            ancestor_state);
    if (h_val == numeric_limits<double>::max()) {
        return DEAD_END;
    } else {
        return static_cast<int>(ceil(h_val - epsilon));
    }
}

bool LandmarkCostPartitioningHeuristic::dead_ends_are_reliable() const
{
    return true;
}

} // namespace landmarks
