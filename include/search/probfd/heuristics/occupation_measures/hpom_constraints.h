#ifndef PROBFD_HEURISTICS_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H
#define PROBFD_HEURISTICS_OCCUPATION_MEASURES_HPOM_CONSTRAINTS_H

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "probfd/task_proxy.h"

#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
namespace occupation_measures {

/**
 * @brief Implements the projection occupation measure heuristic constraints
 * \cite trevizan:etal:icaps-17 .
 */
class HPOMGenerator : public ConstraintGenerator {
    const bool is_maxprob_;
    std::vector<int> offset_;
    mutable std::vector<int> reset_indices_;

public:
    HPOMGenerator();

    explicit HPOMGenerator(bool maxprob);

    virtual void initialize_constraints(
        const std::shared_ptr<ProbabilisticTask>& task,
        lp::LinearProgram& lp) override final;

    void
    update_constraints(const State& state, lp::LPSolver& solver) override final;

    void
    reset_constraints(const State& state, lp::LPSolver& solver) override final;

    static void generate_hpom_lp(
        const ProbabilisticTaskProxy& task_proxy,
        lp::LinearProgram& lp,
        std::vector<int>& offsets,
        bool maxprob);
};

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd

#endif