#include "probfd/heuristics/ucp_heuristic.h"

#include "downward/initial_state_values.h"
#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/tasks/modified_operator_costs_task.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"
#include "probfd/probabilistic_operator_space.h"

#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

UCPHeuristic::UCPHeuristic(
    value_t termination_cost,
    std::vector<ProbabilityAwarePatternDatabase> pdbs)
    : termination_cost_(termination_cost)
    , pdbs_(std::move(pdbs))
{
}

UCPHeuristic::~UCPHeuristic() = default;

value_t UCPHeuristic::evaluate(const State& state) const
{
    value_t value = 0.0_vt;

    for (const auto& pdb : pdbs_) {
        const value_t estimate = pdb.lookup_estimate(state);

        if (estimate == termination_cost_) { return estimate; }

        value += estimate;
    }

    return value;
}

UCPHeuristicFactory::UCPHeuristicFactory(
    utils::Verbosity verbosity,
    std::shared_ptr<PatternCollectionGenerator> generator)
    : verbosity_(verbosity)
    , pattern_collection_generator_(std::move(generator))
{
}

std::unique_ptr<FDRHeuristic>
UCPHeuristicFactory::create_object(const SharedProbabilisticTask& task)
{
    const auto pattern_collection_info =
        pattern_collection_generator_->generate(task);

    const auto& patterns = pattern_collection_info.get_patterns();

    const size_t num_abstractions = patterns.size();

    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    pdbs.reserve(num_abstractions);

    const auto& variables = get_variables(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function =
        get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    std::vector<value_t> costs;
    costs.reserve(operators.get_num_operators());

    for (const ProbabilisticOperatorProxy op : operators) {
        costs.push_back(
            cost_function.get_operator_cost(op.get_id()) /
            static_cast<value_t>(num_abstractions));
    }

    auto uniform_cost_function =
        std::make_shared<extra_tasks::VectorProbabilisticOperatorCostFunction>(
            std::move(costs));

    auto adapted = replace(task, uniform_cost_function);

    const State& initial_state = init_vals.get_initial_state();

    const BlindEvaluator<StateRank> h(
        operators,
        *uniform_cost_function,
        term_costs);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(variables, pattern);
        const StateRank init_rank = pdb.get_abstract_state(initial_state);
        compute_distances(pdb, adapted, init_rank, h);
    }

    return std::make_unique<UCPHeuristic>(
        term_costs.get_non_goal_termination_cost(),
        std::move(pdbs));
}

} // namespace probfd::heuristics
