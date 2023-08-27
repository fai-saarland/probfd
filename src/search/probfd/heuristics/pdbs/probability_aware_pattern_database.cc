#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/multi_policy.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"

#include <algorithm>
#include <deque>
#include <numeric>
#include <set>
#include <sstream>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {

void ProbabilityAwarePatternDatabase::compute_value_table(
    ProjectionStateSpace& mdp,
    const StateRankEvaluator& h,
    StateRank init_state,
    double max_time)
{
    pdbs::compute_value_table(mdp, h, init_state, max_time, value_table);

#ifndef NDEBUG
    std::cout << "(II) Pattern " << ranking_function_.get_pattern() << ":\n"
              << "     value s0: " << value_table[init_state.id] << std::endl;
#endif
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
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
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    FDRSimpleCostFunction& task_cost_function,
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
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        heuristic,
        ranking_function_.get_abstract_rank(initial_state),
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
    compute_value_table(mdp, heuristic, initial_state, max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    FDRSimpleCostFunction& task_cost_function,
    const State& initial_state,
    bool operator_pruning,
    double max_time)
    : ProbabilityAwarePatternDatabase(
          task_proxy,
          pdb.get_pattern(),
          task_cost_function,
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
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& pdb,
    int add_var,
    FDRSimpleCostFunction& task_cost_function,
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
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        ranking_function_.get_abstract_rank(initial_state),
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
        IncrementalPPDBEvaluator(pdb, &ranking_function_, add_var),
        initial_state,
        max_time);
}

ProbabilityAwarePatternDatabase::ProbabilityAwarePatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    FDRSimpleCostFunction& task_cost_function,
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
        ranking_function_,
        task_cost_function,
        operator_pruning,
        timer.get_remaining_time());
    compute_value_table(
        mdp,
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            task_cost_function.get_non_goal_termination_cost()),
        ranking_function_.get_abstract_rank(initial_state),
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
        MergeEvaluator(
            ranking_function_,
            left,
            right,
            mdp.get_non_goal_termination_cost()),
        initial_state,
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
    return value_table[s.id];
}

StateRank
ProbabilityAwarePatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.get_abstract_rank(s);
}

std::unique_ptr<ProjectionMultiPolicy>
ProbabilityAwarePatternDatabase::compute_optimal_projection_policy(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard) const
{
    return pdbs::compute_optimal_projection_policy(
        mdp,
        initial_state,
        value_table,
        rng,
        wildcard);
}

std::unique_ptr<ProjectionMultiPolicy>
ProbabilityAwarePatternDatabase::compute_greedy_projection_policy(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    utils::RandomNumberGenerator& rng,
    bool wildcard) const
{
    return pdbs::compute_greedy_projection_policy(
        mdp,
        initial_state,
        value_table,
        rng,
        wildcard);
}

void ProbabilityAwarePatternDatabase::compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::vector<value_t>& saturated_costs) const
{
    pdbs::compute_saturated_costs(state_space, value_table, saturated_costs);
}

void ProbabilityAwarePatternDatabase::dump_graphviz(
    const ProbabilisticTaskProxy& task_proxy,
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    std::ostream& out,
    bool transition_labels) const
{
    ProjectionOperatorToString op_names(task_proxy);

    auto sts = [this, &mdp](StateRank x) {
        std::ostringstream out;
        out.precision(3);

        const value_t value = value_table[x.id];
        std::string value_text =
            value != INFINITE_VALUE ? std::to_string(value) : "&infin";

        out << x.id << "\\n"
            << "h = " << value_text;

        if (lookup_estimate(x) == mdp.get_non_goal_termination_cost()) {
            out << "(dead)";
        }

        out << std::endl;

        return out.str();
    };

    auto ats = [=](const ProjectionOperator* const& op) {
        return transition_labels ? op_names(op) : "";
    };

    graphviz::dump_state_space_dot_graph<StateRank, const ProjectionOperator*>(
        out,
        initial_state,
        &mdp,
        nullptr,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
