#ifndef PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINT_GENERATOR_H
#define PROBFD_OCCUPATION_MEASURES_PHO_CONSTRAINT_GENERATOR_H

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

class PHOConstraintGenerator : public ConstraintGenerator {
    pdbs::PPDBCollection pdbs_;
    bool use_saturated_costs;

public:
    explicit PHOConstraintGenerator(
        pdbs::PPDBCollection pdbs,
        bool use_saturated_costs);

    void initialize_constraints(
        const SharedProbabilisticTask& task,
        downward::lp::LinearProgram& lp) final;

    void update_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;
    void reset_constraints(
        const downward::State& state,
        downward::lp::LPSolver& solver) final;
};

class PHOConstraintGeneratorFactory : public ConstraintGeneratorFactory {
    std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;
    bool use_saturated_costs;

public:
    explicit PHOConstraintGeneratorFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        bool use_saturated_costs);

    std::unique_ptr<ConstraintGenerator> construct_constraint_generator(
        const SharedProbabilisticTask& task) override;
};

} // namespace probfd::occupation_measures

#endif
