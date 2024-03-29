#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURES_HROC_CONSTRAINTS_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURES_HROC_CONSTRAINTS_H

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace occupation_measures {

/**
 * @brief Implements the regrouped operator counting heuristic constraints
 * \cite trevizan:etal:icaps-17 .
 */
class HROCGenerator : public ConstraintGenerator {
    std::vector<std::size_t> ncc_offsets_;

public:
    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif // __REGROUPED_OPERATOR_COUNTING_HEURISTIC_H__