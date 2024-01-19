#ifndef PROBFD_PDBS_EVALUATORS_H
#define PROBFD_PDBS_EVALUATORS_H

#include "probfd/pdbs/types.h"

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

// Forward Declarations
namespace pdbs {
class PatternDatabase;
}

namespace probfd::pdbs {
class StateRankingFunction;
class ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::pdbs {

/// Type alias for heuristics for projection states.
using StateRankEvaluator = Evaluator<StateRank>;

class PDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb_;

public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    [[nodiscard]]
    value_t evaluate(StateRank state) const override;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
    const ::pdbs::PatternDatabase& pdb_;

public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    [[nodiscard]]
    value_t evaluate(StateRank state) const override;
};

class IncrementalPPDBEvaluator : public StateRankEvaluator {
    const ProbabilityAwarePatternDatabase& pdb_;

    int left_multiplier_;
    int right_multiplier_;
    int domain_size_;

public:
    explicit IncrementalPPDBEvaluator(
        const ProbabilityAwarePatternDatabase& pdb,
        const StateRankingFunction* mapper,
        int add_var);

    [[nodiscard]]
    value_t evaluate(StateRank state) const override;

private:
    [[nodiscard]]
    StateRank to_parent_state(StateRank state) const;
};

class MergeEvaluator : public StateRankEvaluator {
    const StateRankingFunction& mapper_;
    const ProbabilityAwarePatternDatabase& left_;
    const ProbabilityAwarePatternDatabase& right_;
    const value_t termination_cost_;

public:
    MergeEvaluator(
        const StateRankingFunction& mapper,
        const ProbabilityAwarePatternDatabase& left,
        const ProbabilityAwarePatternDatabase& right,
        value_t termination_cost);

    [[nodiscard]]
    value_t evaluate(StateRank state) const override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_EVALUATORS_H
