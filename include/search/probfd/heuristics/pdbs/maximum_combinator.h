#ifndef PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_H

#include "probfd/heuristics/pdbs/incremental_pdb_combinator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaximumCombinator : public IncrementalPDBCombinator {
public:
    void update(const PPDBCollection&) override;

    int count_improvements_if_added(
        const PPDBCollection& pdbs,
        const ProbabilityAwarePatternDatabase& new_pdb,
        const std::vector<State>& states,
        value_t termination_cost) override;

    value_t evaluate(
        const PPDBCollection& database,
        const State& state,
        value_t termination_cost);

    void print_statistics(utils::LogProxy log) const override {}
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_MAXIMUM_COMBINATOR_H
