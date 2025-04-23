#ifndef PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H
#define PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H

#include "probfd/occupation_measures/constraint_generator.h"

#include "probfd/pdbs/types.h"

#include <memory>

// Forward Declarations
namespace downward {
class State;
}

namespace downward::lp {
struct LPVariable;
class LPConstraint;
class LPSolver;
} // namespace downward::lp

namespace probfd::pdbs {
class PatternCollectionGenerator;
}

namespace probfd::occupation_measures {

class PHOGenerator : public ConstraintGenerator {
    pdbs::PPDBCollection pdbs_;

public:
    explicit PHOGenerator(pdbs::PPDBCollection pdbs);

    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        downward::lp::LinearProgram& lp) final;

    void update_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;
    void reset_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;
};

class PHOGeneratorFactory : public ConstraintGeneratorFactory {
    std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;

public:
    explicit PHOGeneratorFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    std::unique_ptr<ConstraintGenerator> construct_constraint_generator(
        const std::shared_ptr<ProbabilisticTask>& task) override;
};

} // namespace probfd::occupation_measures

#endif // PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINTS_H
