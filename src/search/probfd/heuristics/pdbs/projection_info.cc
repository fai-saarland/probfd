#include "probfd/heuristics/pdbs/projection_info.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

ProjectionInfo::ProjectionInfo() = default;

ProjectionInfo::ProjectionInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    const State& initial_state,
    const AbstractionEvaluator& heuristic,
    bool prune_eqv_operators,
    double max_time)
    : pdb(new ProbabilityAwarePatternDatabase(
          task_proxy,
          task_cost_function,
          std::move(pattern),
          initial_state,
          heuristic,
          mdp,
          prune_eqv_operators,
          max_time))
{
}

ProjectionInfo::ProjectionInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    const State& initial_state,
    const ProbabilityAwarePatternDatabase& pdb,
    bool prune_eqv_operators,
    double max_time)
    : pdb(new ProbabilityAwarePatternDatabase(
          task_proxy,
          task_cost_function,
          std::move(pattern),
          initial_state,
          pdb,
          mdp,
          prune_eqv_operators,
          max_time))
{
}

ProjectionInfo::ProjectionInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    const State& initial_state,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    bool prune_eqv_operators,
    double max_time)
    : pdb(new ProbabilityAwarePatternDatabase(
          task_proxy,
          task_cost_function,
          std::move(pattern),
          initial_state,
          left,
          right,
          mdp,
          prune_eqv_operators,
          max_time))
{
}

ProjectionInfo::ProjectionInfo(ProjectionInfo&&) noexcept = default;
ProjectionInfo& ProjectionInfo::operator=(ProjectionInfo&&) noexcept = default;
ProjectionInfo::~ProjectionInfo() = default;

/// Get the pattern of the pattern database.
const Pattern& ProjectionInfo::get_pattern() const
{
    return pdb->get_pattern();
}

/// Get the abstraction mapping of the pattern database.
const StateRankingFunction& ProjectionInfo::get_state_ranking_function() const
{
    return pdb->get_state_ranking_function();
}

/// Get the abstraction mapping of the pattern database.
const std::vector<value_t>& ProjectionInfo::get_value_table() const
{
    return pdb->get_value_table();
}

/// Get the number of states in this PDB's projection.
unsigned int ProjectionInfo::num_states() const
{
    return pdb->num_states();
}

/// Compute the state rank of the abstract state of an input state.
AbstractStateIndex ProjectionInfo::get_abstract_state(const State& state) const
{
    return pdb->get_abstract_state(state);
}

/// Get the optimal state value of the abstract state corresponding to the
/// input state.
value_t ProjectionInfo::lookup_estimate(const State& state) const
{
    return pdb->lookup_estimate(state);
}

/// Look up the estimate of an abstract state specified by state rank in
/// the lookup table.
value_t ProjectionInfo::lookup_estimate(AbstractStateIndex index) const
{
    return pdb->lookup_estimate(index);
}

bool ProjectionInfo::is_goal(AbstractStateIndex s) const
{
    return mdp->is_goal(s);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd