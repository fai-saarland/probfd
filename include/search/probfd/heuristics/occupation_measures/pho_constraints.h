#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTICS_PHO_GENERATOR_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURE_HEURISTICS_PHO_GENERATOR_H

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "probfd/heuristics/pdbs/pattern_generator.h"
#include "probfd/heuristics/pdbs/types.h"

#include <memory>
#include <vector>

class State;

namespace plugins {
class Options;
}

namespace lp {
struct LPVariable;
class LPConstraint;
class LPSolver;
} // namespace lp

namespace probfd {
namespace heuristics {
namespace occupation_measures {

class PHOGenerator : public ConstraintGenerator {
    std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;
    std::shared_ptr<pdbs::PPDBCollection> pdbs_;

public:
    explicit PHOGenerator(const plugins::Options& opts);
    explicit PHOGenerator(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator);

    void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif