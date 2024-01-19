#ifndef PROBFD_OCCUPATION_MEASURES_HROC_CONSTRAINTS_H
#define PROBFD_OCCUPATION_MEASURES_HROC_CONSTRAINTS_H

#include "probfd/occupation_measures/constraint_generator.h"

#include "probfd/fdr_types.h"

#include <cstddef>
#include <memory>
#include <vector>

// Forward Declarations
class State;

namespace lp {
class LPSolver;
class LinearProgram;
} // namespace lp

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::occupation_measures {

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
        lp::LinearProgram& lp) final;

    void update_constraints(const State& state, lp::LPSolver& solver) final;
    void reset_constraints(const State& state, lp::LPSolver& solver) final;
};

} // namespace probfd::occupation_measures

#endif // PROBFD_OCCUPATION_MEASURES_HROC_CONSTRAINTS_H
