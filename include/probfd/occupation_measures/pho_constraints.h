#ifndef PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H
#define PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H

#include "probfd/occupation_measures/constraint_generator.h"

#include "probfd/pdbs/types.h"

#include <memory>

// Forward Declarations
class State;

namespace lp {
struct LPVariable;
class LPConstraint;
class LPSolver;
} // namespace lp

namespace probfd::pdbs {
class PatternCollectionGenerator;
}

namespace probfd::occupation_measures {

class PHOGenerator : public ConstraintGenerator {
    std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;
    std::shared_ptr<pdbs::PPDBCollection> pdbs_;

public:
    explicit PHOGenerator(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function,
        lp::LinearProgram& lp) final;

    void update_constraints(const State& state, lp::LPSolver& solver) final;
    void reset_constraints(const State& state, lp::LPSolver& solver) final;
};

} // namespace probfd::occupation_measures

#endif // PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H
