#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/distances.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include <algorithm>

namespace probfd {
namespace pdbs {

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    Pattern pattern,
    value_t dead_end_cost)
    : ranking_function_(task_proxy.get_variables(), std::move(pattern))
    , value_table(ranking_function_.num_states(), dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    StateRankingFunction ranking_function,
    value_t dead_end_cost)
    : ranking_function_(std::move(ranking_function))
    , value_table(ranking_function_.num_states(), dead_end_cost)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    const State& initial_state,
    bool operator_pruning,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          std::move(pattern),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);
    ProjectionStateSpace mdp(
        task_proxy,
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        heuristic,
        value_table,
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(mdp, initial_state, heuristic, value_table, max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ::pdbs::PatternDatabase& pdb,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          task_cost_function,
          pdb.get_pattern(),
          initial_state,
          operator_pruning,
          task_cost_function.get_non_goal_termination_cost() == INFINITE_VALUE
              ? static_cast<const StateRankEvaluator&>(PDBEvaluator(pdb))
              : static_cast<const StateRankEvaluator&>(
                    DeadendPDBEvaluator(pdb)),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ::pdbs::PatternDatabase& pdb,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          mdp,
          std::move(ranking_function),
          initial_state,
          mdp.get_non_goal_termination_cost() == INFINITE_VALUE
              ? static_cast<const StateRankEvaluator&>(PDBEvaluator(pdb))
              : static_cast<const StateRankEvaluator&>(
                    DeadendPDBEvaluator(pdb)),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          extended_pattern(pdb.get_pattern(), add_var),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace mdp(
        task_proxy,
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        value_table,
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(
        mdp,
        initial_state,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        value_table,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()),
          task_cost_function.get_non_goal_termination_cost())
{
    utils::CountdownTimer timer(max_time);

    ProjectionStateSpace mdp(
        task_proxy,
        task_cost_function,
        ranking_function_,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function_.get_abstract_rank(initial_state),
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            task_cost_function.get_non_goal_termination_cost()),
        value_table,
        timer.get_remaining_time());
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          std::move(ranking_function),
          mdp.get_non_goal_termination_cost())
{
    compute_value_table(
        mdp,
        initial_state,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            mdp.get_non_goal_termination_cost()),
        value_table,
        max_time);
}

const Pattern& ProbabilityAwarePatternDatabase::get_pattern() const
{
    return ranking_function_.get_pattern();
}

const StateRankingFunction&
ProbabilityAwarePatternDatabase::get_state_ranking_function() const
{
    return ranking_function_;
}

const std::vector<value_t>&
ProbabilityAwarePatternDatabase::get_value_table() const
{
    return value_table;
}

unsigned int ProbabilityAwarePatternDatabase::num_states() const
{
    return ranking_function_.num_states();
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
    return ranking_function_.get_abstract_rank(s);
}

} // namespace pdbs
} // namespace probfd
