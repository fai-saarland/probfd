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

namespace probfd::pdbs {

namespace {
void compute_value_table(
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    const StateRankingFunction& ranking_function,
    bool operator_pruning,
    utils::CountdownTimer& timer,
    const State& initial_state,
    const Evaluator<StateRank>& heuristic,
    std::span<value_t> value_table)
{
    ProjectionStateSpace mdp(
        task_proxy,
        std::move(task_cost_function),
        ranking_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        ranking_function.get_abstract_rank(initial_state),
        heuristic,
        value_table,
        timer.get_remaining_time());
}
} // namespace

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    Pattern pattern)
    : ranking_function_(task_proxy.get_variables(), std::move(pattern))
    , value_table_(
          ranking_function_.num_states(),
          std::numeric_limits<value_t>::quiet_NaN())
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    StateRankingFunction ranking_function)
    : ranking_function_(std::move(ranking_function))
    , value_table_(
          ranking_function_.num_states(),
          std::numeric_limits<value_t>::quiet_NaN())
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    StateRankingFunction ranking_function,
    std::vector<value_t> value_table)
    : ranking_function_(std::move(ranking_function))
    , value_table_(std::move(value_table))
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    Pattern pattern,
    const State& initial_state,
    bool operator_pruning,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(task_proxy, std::move(pattern))
{
    utils::CountdownTimer timer(max_time);

    compute_value_table(
        task_proxy,
        std::move(task_cost_function),
        ranking_function_,
        operator_pruning,
        timer,
        initial_state,
        heuristic,
        value_table_);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    StateRank initial_state,
    const StateRankEvaluator& heuristic,
    double max_time)
    : ProbabilityAwarePatternDatabase(std::move(ranking_function))
{
    compute_value_table(mdp, initial_state, heuristic, value_table_, max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
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
          task_cost_function->get_non_goal_termination_cost() == INFINITE_VALUE
              ? static_cast<const StateRankEvaluator&>(PDBEvaluator(pdb))
              : static_cast<const StateRankEvaluator&>(
                    DeadendPDBEvaluator(pdb)),
          max_time)
{
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    const ::pdbs::PatternDatabase& pdb,
    StateRank initial_state,
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
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          extended_pattern(pdb.get_pattern(), add_var))
{
    utils::CountdownTimer timer(max_time);

    compute_value_table(
        task_proxy,
        std::move(task_cost_function),
        ranking_function_,
        operator_pruning,
        timer,
        initial_state,
        IncrementalPPDBEvaluator(
            pdb.get_value_table(),
            ranking_function_,
            add_var),
        value_table_);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    StateRank initial_state,
    double max_time)
    : ProbabilityAwarePatternDatabase(std::move(ranking_function))
{
    compute_value_table(
        mdp,
        initial_state,
        IncrementalPPDBEvaluator(
            pdb.get_value_table(),
            ranking_function_,
            add_var),
        value_table_,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()))
{
    utils::CountdownTimer timer(max_time);

    const auto term_cost = task_cost_function->get_non_goal_termination_cost();

    compute_value_table(
        task_proxy,
        std::move(task_cost_function),
        ranking_function_,
        operator_pruning,
        timer,
        initial_state,
        MergeEvaluator(ranking_function_, left, right, term_cost),
        value_table_);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    ProjectionStateSpace& mdp,
    StateRankingFunction ranking_function,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    StateRank initial_state,
    double max_time)
    : ProbabilityAwarePatternDatabase(std::move(ranking_function))
{
    compute_value_table(
        mdp,
        initial_state,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            mdp.get_non_goal_termination_cost()),
        value_table_,
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
    return value_table_;
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
    return value_table_[s];
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.get_abstract_rank(s);
}

} // namespace probfd::pdbs
