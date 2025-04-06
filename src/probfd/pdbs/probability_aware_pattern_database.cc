#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/utils.h"

#include "probfd/abstractions/distances.h"

#include "probfd/cost_function.h"
#include "probfd/task_proxy.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include <limits>
#include <utility>

using namespace downward;

namespace probfd::pdbs {

static void compute_distances(
    std::span<value_t> value_table,
    const StateRankingFunction& ranking_function,
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    StateRank abstract_initial_state,
    const Heuristic<StateRank>& heuristic,
    bool operator_pruning,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace mdp(
        task_proxy,
        std::move(task_cost_function),
        ranking_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        abstract_initial_state,
        heuristic,
        value_table,
        timer.get_remaining_time());
}

void compute_distances(
    ProbabilityAwarePatternDatabase& pdb,
    ProjectionStateSpace& mdp,
    StateRank abstract_initial_state,
    const Heuristic<StateRank>& heuristic,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    compute_value_table(
        mdp,
        abstract_initial_state,
        heuristic,
        pdb.value_table,
        timer.get_remaining_time());
}

void compute_distances(
    ProbabilityAwarePatternDatabase& pdb,
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    StateRank abstract_initial_state,
    const Heuristic<StateRank>& heuristic,
    bool operator_pruning,
    double max_time)
{
    compute_distances(
        pdb.value_table,
        pdb.ranking_function,
        task_proxy,
        std::move(task_cost_function),
        abstract_initial_state,
        heuristic,
        operator_pruning,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const VariablesProxy& variables,
    Pattern pattern)
    : ranking_function(variables, std::move(pattern))
    , value_table(
          ranking_function.num_states(),
          std::numeric_limits<value_t>::quiet_NaN())
{
}

const Pattern& ProbabilityAwarePatternDatabase::get_pattern() const
{
    return ranking_function.get_pattern();
}

unsigned int ProbabilityAwarePatternDatabase::num_states() const
{
    return ranking_function.num_states();
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(const State& s) const
{
    return lookup_estimate(get_abstract_state(s));
}

value_t ProbabilityAwarePatternDatabase::lookup_estimate(StateRank s) const
{
    return value_table[s];
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function.get_abstract_rank(s);
}

} // namespace probfd::pdbs
