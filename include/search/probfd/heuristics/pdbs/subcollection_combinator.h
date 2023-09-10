#ifndef PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_COMBINATOR_H

#include "probfd/heuristics/pdbs/incremental_pdb_combinator.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

class SubCollectionCombinator : public IncrementalPDBCombinator {
protected:
    std::vector<PatternSubCollection> subcollections_;

public:
    int count_improvements_if_added(
        const PPDBCollection& pdbs,
        const ProbabilityAwarePatternDatabase& new_pdb,
        const std::vector<State>& states,
        value_t termination_cost) override;

    value_t evaluate(
        const PPDBCollection& database,
        const State& state,
        value_t termination_cost) override;

    void print_statistics(utils::LogProxy log) const override;

    double run_dominance_pruning(
        ProbabilisticTaskProxy task_proxy,
        PPDBCollection& pdbs,
        double max_time,
        utils::LogProxy log);

private:
    virtual std::vector<PatternSubCollection> update_with_pdbs(
        const PPDBCollection& pdbs,
        const ProbabilityAwarePatternDatabase& new_pdb) = 0;

    virtual value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const = 0;

    virtual value_t combine(value_t left, value_t right) const = 0;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_COMBINATOR_H
