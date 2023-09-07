#include "probfd/heuristics/pdbs/evaluators.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"

#include <limits>
#include <memory>
#include <ranges>

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBEvaluator::PDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

value_t PDBEvaluator::evaluate(StateRank state_rank) const
{
    int deterministic_val = pdb.get_value_for_index(state_rank);

    if (deterministic_val == std::numeric_limits<int>::max()) {
        return INFINITE_VALUE;
    }

    return static_cast<value_t>(deterministic_val);
}

DeadendPDBEvaluator::DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

value_t DeadendPDBEvaluator::evaluate(StateRank state) const
{
    int deterministic_val = pdb.get_value_for_index(state);

    if (deterministic_val == std::numeric_limits<int>::max()) {
        return 1_vt;
    }

    return 0_vt;
}

IncrementalPPDBEvaluator::IncrementalPPDBEvaluator(
    const StateRankingFunction& abstraction_function,
    const std::vector<
        std::reference_wrapper<const ProbabilityAwarePatternDatabase>>& pdbs)
    : estimates(abstraction_function.num_states())
{
    using namespace std::views;

    abstraction_function.convert_value_table(pdbs.front(), estimates);

    if (pdbs.size() == 1) return;

    std::vector<value_t> temp(abstraction_function.num_states());
    for (const ProbabilityAwarePatternDatabase& pdb : pdbs | drop(1)) {
        abstraction_function.convert_value_table(pdb, temp);

        for (auto [left, right] : std::views::zip(estimates, temp)) {
            left = std::max(left, right);
        }
    }
}

value_t IncrementalPPDBEvaluator::evaluate(StateRank state) const
{
    return estimates[state];
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
