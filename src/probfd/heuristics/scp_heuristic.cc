#include "probfd/heuristics/scp_heuristic.h"

#include "downward/initial_state_values.h"
#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/saturation.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/task_utils/task_properties.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/tasks/range_operator_cost_function.h"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

SCPHeuristicFactory::SCPHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> pattern_collection_generator,
    OrderingStrategy ordering,
    int random_seed,
    utils::Verbosity verbosity)
    : pattern_collection_generator_(std::move(pattern_collection_generator))
    , ordering_(ordering)
    , random_seed_(random_seed)
    , verbosity_(verbosity)
{
}

std::unique_ptr<FDRHeuristic>
SCPHeuristicFactory::create_object(const SharedProbabilisticTask& task)
{
    auto pattern_collection_info =
        pattern_collection_generator_->generate(task);

    auto patterns = pattern_collection_info.get_patterns();

    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    pdbs.reserve(patterns.size());

    const std::shared_ptr<utils::RandomNumberGenerator> rng =
        utils::get_rng(random_seed_);

    switch (ordering_) {
    case RANDOM: rng->shuffle(patterns); break;

    case SIZE_ASC:
        std::ranges::stable_sort(patterns, std::less<>(), &Pattern::size);
        break;

    case SIZE_DESC:
        std::ranges::stable_sort(patterns, std::greater<>(), &Pattern::size);
        break;

    case INHERIT:
    default: break;
    }

    const auto& variables = get_variables(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function = get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    const size_t num_operators = operators.size();

    std::vector<value_t> saturated_costs(num_operators);

    const State& initial_state = init_vals.get_initial_state();

    std::vector<value_t> costs;
    costs.reserve(operators.size());

    for (const ProbabilisticOperatorProxy op : operators) {
        costs.push_back(cost_function.get_operator_cost(op.get_id()));
    }

    auto running_cost_function =
        downward::extra_tasks::make_shared_range_cf(std::move(costs));

    auto adapted = replace(task, running_cost_function);

    BlindEvaluator<StateRank> h(operators, *running_cost_function, term_costs);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(variables, pattern);

        ProjectionStateSpace state_space(adapted, pdb.ranking_function, false);

        compute_distances(
            pdb,
            state_space,
            pdb.get_abstract_state(initial_state),
            h);

        compute_saturated_costs(state_space, pdb.value_table, saturated_costs);

        for (size_t i = 0; i != num_operators; ++i) {
            const auto new_cost =
                (*running_cost_function)[i] - saturated_costs[i];
            assert(!is_approx_less(new_cost, 0.0_vt, 0.0001));

            // Avoid floating point imprecision. The PDB implementation is not
            // stable with respect to action costs very close to zero.
            if (is_approx_equal(new_cost, 0.0_vt, 0.0001)) {
                (*running_cost_function)[i] = 0.0_vt;
            } else {
                (*running_cost_function)[i] = new_cost;
            }
        }
    }

    return std::make_unique<SCPHeuristic>(
        term_costs.get_non_goal_termination_cost(),
        std::move(pdbs));
}

SCPHeuristic::SCPHeuristic(
    value_t termination_cost,
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : termination_cost_(termination_cost)
    , pdbs_(std::move(pdbs))
{
}

SCPHeuristic::~SCPHeuristic() = default;

value_t SCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost_) { return estimate; }

        value += estimate;
    }

    return value;
}

} // namespace probfd::heuristics
