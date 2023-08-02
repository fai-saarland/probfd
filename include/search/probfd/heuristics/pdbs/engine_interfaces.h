#ifndef PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/types.h"

#include "probfd/heuristics/pdbs/state_rank.h"

#include <vector>

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

class StateRankingFunction;
class ProbabilityAwarePatternDatabase;

/// Type alias for heuristics for projection states.
using StateRankEvaluator = engine_interfaces::Evaluator<StateRank>;

class PDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb;

public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb;

public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;
};

class IncrementalPPDBEvaluator : public StateRankEvaluator {
    const ProbabilityAwarePatternDatabase& pdb;

    int left_multiplier;
    int right_multiplier;
    int domain_size;

public:
    explicit IncrementalPPDBEvaluator(
        const ProbabilityAwarePatternDatabase& pdb,
        const StateRankingFunction* mapper,
        int add_var);

    EvaluationResult evaluate(StateRank state) const override;

private:
    StateRank to_parent_state(StateRank state) const;
};

class MergeEvaluator : public StateRankEvaluator {
    const StateRankingFunction& mapper;
    const ProbabilityAwarePatternDatabase& left;
    const ProbabilityAwarePatternDatabase& right;

public:
    MergeEvaluator(
        const StateRankingFunction& mapper,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right);

    EvaluationResult evaluate(StateRank state) const override;

private:
    StateRank convert(
        StateRank state_rank,
        const StateRankingFunction& refined_mapping,
        const StateRankingFunction& coarser_mapping) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__