#ifndef PROBFD_HEURISTICS_PDBS_EVALUATORS_H
#define PROBFD_HEURISTICS_PDBS_EVALUATORS_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/evaluator.h"
#include "probfd/fdr_types.h"

#include <concepts>
#include <vector>

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class StateRankingFunction;
class ProbabilityAwarePatternDatabase;

/// Type alias for heuristics for projection states.
using StateRankEvaluator = Evaluator<StateRank>;

class PDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb;

public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    value_t evaluate(StateRank state) const override;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb;

public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    value_t evaluate(StateRank state) const override;
};

class IncrementalPPDBEvaluator : public StateRankEvaluator {
    std::vector<value_t> estimates;

public:
    explicit IncrementalPPDBEvaluator(
        const StateRankingFunction& mapper,
        const std::vector<
            std::reference_wrapper<const ProbabilityAwarePatternDatabase>>&
            pdbs);

    explicit IncrementalPPDBEvaluator(
        const StateRankingFunction& mapper,
        const std::same_as<ProbabilityAwarePatternDatabase> auto&... pdbs)
        : IncrementalPPDBEvaluator(mapper, {std::ref(pdbs)...})
    {
    }

    value_t evaluate(StateRank state) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_EVALUATORS_H
