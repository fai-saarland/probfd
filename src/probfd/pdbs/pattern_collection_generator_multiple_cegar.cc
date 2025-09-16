#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

#include "probfd/pdbs/cegar/single_cegar.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/abstractions/distances.h"

#include "probfd/probabilistic_task.h"

#include "downward/abstract_task.h"
#include "downward/initial_state_values.h"
#include "downward/utils/countdown_timer.h"

using namespace std;
using namespace downward;

namespace probfd::pdbs {

using namespace cegar;

PatternCollectionGeneratorMultipleCegar::
    PatternCollectionGeneratorMultipleCegar(
        value_t convergence_epsilon,
        std::shared_ptr<FlawFindingStrategy> flaw_strategy,
        bool use_wildcard_policies,
        int max_pdb_size,
        int max_collection_size,
        utils::Duration pattern_generation_max_time,
        utils::Duration total_max_time,
        utils::Duration stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        bool use_saturated_costs,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        utils::Verbosity verbosity)
    : PatternCollectionGeneratorMultiple(
          max_pdb_size,
          max_collection_size,
          pattern_generation_max_time,
          total_max_time,
          stagnation_limit,
          blacklist_trigger_percentage,
          enable_blacklist_on_stagnation,
          use_saturated_costs,
          std::move(rng),
          "CEGAR",
          verbosity)
    , convergence_epsilon_(convergence_epsilon)
    , flaw_strategy_(std::move(flaw_strategy))
    , use_wildcard_policies_(use_wildcard_policies)
{
}

ProjectionTransformation
PatternCollectionGeneratorMultipleCegar::compute_pattern(
    int max_pdb_size,
    utils::Duration max_time,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const SharedProbabilisticTask& task,
    const FactPair& goal,
    unordered_set<int>&& blacklisted_variables)
{
    utils::CountdownTimer timer(max_time);

    // Start with a solution of the trivial abstraction
    ProjectionTransformation transformation(
        task,
        {goal.var},
        false,
        timer.get_remaining_time());

    const auto& operators = get_operators(task);
    const auto& cost_function = get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    const State initial_state = get_init(task).get_initial_state();

    compute_value_table(
        *transformation.projection,
        transformation.pdb.get_abstract_state(initial_state),
        heuristics::BlindHeuristic<StateRank>(
            operators,
            cost_function,
            term_costs),
        transformation.pdb.value_table,
        timer.get_remaining_time(),
        convergence_epsilon_);

    run_cegar_loop(
        transformation,
        task,
        initial_state,
        convergence_epsilon_,
        *flaw_strategy_,
        std::move(blacklisted_variables),
        max_pdb_size,
        *rng,
        use_wildcard_policies_,
        max_time,
        log_);

    return transformation;
}

} // namespace probfd::pdbs
