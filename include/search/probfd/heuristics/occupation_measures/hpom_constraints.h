#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "probfd/task_proxy.h"

#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace occupation_measures {

/**
 * @brief Implements the projection occupation measure heuristic constraints
 * \cite trevizan:etal:icaps-17 .
 */
class HPOMGenerator : public ConstraintGenerator {
    std::vector<int> offset_;
    mutable std::vector<int> reset_indices_;

public:
    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<TaskCostFunction>& task_cost_function,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;

    static void generate_hpom_lp(
        const ProbabilisticTaskProxy& task_proxy,
        const TaskCostFunction& task_cost_function,
        lp::LinearProgram& lp,
        std::vector<int>& offsets);
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif