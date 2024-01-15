#ifndef PROBFD_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H
#define PROBFD_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H

#include "probfd/occupation_measures/constraint_generator.h"

#include "probfd/probabilistic_task.h"

#include "probfd/fdr_types.h"

#include <memory>
#include <vector>

class State;

namespace lp {
class LPSolver;
class LinearProgram;
} // namespace lp

namespace probfd {
class ProbabilisticTaskProxy;

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
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;

    static void generate_hpom_lp(
        const ProbabilisticTaskProxy& task_proxy,
        const FDRCostFunction& task_cost_function,
        lp::LinearProgram& lp,
        std::vector<int>& offsets);
};

} // namespace occupation_measures
} // namespace probfd

#endif // PROBFD_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H
