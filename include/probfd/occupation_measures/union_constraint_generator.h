#ifndef PROBFD_OCCUPATION_MEASURES_UNION_CONSTRAINT_GENERATOR_H
#define PROBFD_OCCUPATION_MEASURES_UNION_CONSTRAINT_GENERATOR_H

#include "probfd/occupation_measures/constraint_generator.h"

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

class UnionConstraintGenerator : public ConstraintGenerator {
    std::vector<std::unique_ptr<ConstraintGenerator>> generators_;

public:
    explicit UnionConstraintGenerator(
        std::vector<std::unique_ptr<ConstraintGenerator>> generators);

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

class UnionConstraintGeneratorFactory : public ConstraintGeneratorFactory {
    std::vector<std::shared_ptr<ConstraintGeneratorFactory>>
        generators_factories_;

public:
    explicit UnionConstraintGeneratorFactory(
        std::vector<std::shared_ptr<ConstraintGeneratorFactory>>
            generators_factories);

    std::unique_ptr<ConstraintGenerator> construct_constraint_generator(
        const SharedProbabilisticTask& task) override;
};

} // namespace probfd::occupation_measures

#endif
