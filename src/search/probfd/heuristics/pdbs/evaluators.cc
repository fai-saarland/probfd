#include "probfd/heuristics/pdbs/evaluators.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"

#include <limits>
#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBEvaluator::PDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

value_t PDBEvaluator::evaluate(StateRank state) const
{
    int deterministic_val = pdb.get_value_for_index(state.id);

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
    int deterministic_val = pdb.get_value_for_index(state.id);

    if (deterministic_val == std::numeric_limits<int>::max()) {
        return 1_vt;
    }

    return 0_vt;
}

IncrementalPPDBEvaluator::IncrementalPPDBEvaluator(
    const ProbabilityAwarePatternDatabase& pdb,
    const StateRankingFunction* mapper,
    int add_var)
    : pdb(pdb)
{
    const Pattern& pattern = mapper->get_pattern();
    auto it = std::ranges::lower_bound(pattern, add_var);
    assert(it != pattern.end());
    auto idx = std::distance(pattern.begin(), it);

    this->domain_size = mapper->get_domain_size(idx);
    this->left_multiplier = mapper->get_multiplier(idx);
    this->right_multiplier =
        static_cast<unsigned int>(idx + 1) < mapper->num_vars()
            ? mapper->get_multiplier(idx + 1)
            : mapper->num_states();
}

value_t IncrementalPPDBEvaluator::evaluate(StateRank state) const
{
    return pdb.lookup_estimate(to_parent_state(state));
}

StateRank IncrementalPPDBEvaluator::to_parent_state(StateRank state) const
{
    int left = state.id % left_multiplier;
    int right = state.id - (state.id % right_multiplier);
    return StateRank(left + right / domain_size);
}

MergeEvaluator::MergeEvaluator(
    const StateRankingFunction& mapper,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    value_t termination_cost)
    : mapper(mapper)
    , left(left)
    , right(right)
    , termination_cost(termination_cost)
{
}

value_t MergeEvaluator::evaluate(StateRank state) const
{
    const StateRank lstate =
        convert(state, mapper, left.get_state_ranking_function());

    auto leval = left.lookup_estimate(lstate);

    if (leval == termination_cost) {
        return leval;
    }

    const StateRank rstate =
        convert(state, mapper, right.get_state_ranking_function());

    auto reval = right.lookup_estimate(rstate);

    if (reval == termination_cost) {
        return reval;
    }

    return std::max(leval, reval);
}

StateRank MergeEvaluator::convert(
    StateRank state_rank,
    const StateRankingFunction& refined_mapping,
    const StateRankingFunction& coarser_mapping) const
{
    const Pattern& larger_pattern = refined_mapping.get_pattern();
    const Pattern& smaller_pattern = coarser_mapping.get_pattern();

    assert(std::ranges::includes(larger_pattern, smaller_pattern));

    StateRank rank(0);

    for (size_t i = 0, j = 0; j != smaller_pattern.size(); ++i, ++j) {
        while (larger_pattern[i] != smaller_pattern[j]) ++i;
        rank.id += coarser_mapping.rank_fact(
            j,
            refined_mapping.value_of(state_rank, i));
    }

    return rank;
}

IncrementalValueTableEvaluator::IncrementalValueTableEvaluator(int initial_size)
    : current_value_table(initial_size, 0_vt)
{
}

value_t IncrementalValueTableEvaluator::evaluate(StateRank state) const
{
    return current_value_table[state.id];
}

void IncrementalValueTableEvaluator::on_refinement(
    const StateRankingFunction& mapper,
    int add_var)
{
    const Pattern& pattern = mapper.get_pattern();
    auto it = std::ranges::lower_bound(pattern, add_var);
    assert(it != pattern.end());
    auto idx = std::distance(pattern.begin(), it);

    int domain_size = mapper.get_domain_size(idx);
    int left_multiplier = mapper.get_multiplier(idx);
    int right_multiplier =
        static_cast<unsigned int>(idx + 1) < mapper.num_vars()
            ? mapper.get_multiplier(idx + 1)
            : mapper.num_states();

    std::vector<value_t> next_value_table(mapper.num_states(), 0_vt);

    for (size_t i = 0; i != mapper.num_states(); ++i) {
        int left = i % left_multiplier;
        int right = i - (i % right_multiplier);
        int parent = left + right / domain_size;
        next_value_table[i] = current_value_table[parent];
    }

    current_value_table = std::move(next_value_table);
}

std::vector<value_t>& IncrementalValueTableEvaluator::get_value_table()
{
    return current_value_table;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
