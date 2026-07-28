#include "probfd/heuristics/ucp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/tasks/range_operator_cost_function.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"
#include "probfd/heuristics/additive_pdb_heuristic.h"
#include "probfd/probabilistic_operator_space.h"

#include "downward/initial_state_values.h"

#include <utility>

using namespace downward;
using namespace probfd::pdbs;

namespace probfd::heuristics {

UCPHeuristicFactory::UCPHeuristicFactory(
    std::shared_ptr<PatternCollectionGenerator> generator)
    : pattern_collection_generator_(std::move(generator))
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
    const auto& cost_function = get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    // Number of abstractions affected by the operator.
    std::vector<std::size_t> num_affected(operators.get_num_operators(), 0);

    for (const ProbabilisticOperatorProxy op : operators) {
        std::set<int> affected_vars;
        task_properties::get_affected_vars(
            op,
            std::inserter(affected_vars, affected_vars.begin()));

        for (const Pattern& pattern : patterns) {
            if (utils::have_common_element(pattern, affected_vars)) {
                ++num_affected[op.get_id()];
            }
        }
    }

    const State& initial_state = init_vals.get_initial_state();

    for (const Pattern& pattern : patterns) {
        std::vector costs(operators.get_num_operators(), 0_vt);

        for (const ProbabilisticOperatorProxy op : operators) {
            std::set<int> affected_vars;
            task_properties::get_affected_vars(
                op,
                std::inserter(affected_vars, affected_vars.begin()));

            if (utils::have_common_element(pattern, affected_vars)) {
                costs[op.get_id()] =
                    cost_function.get_operator_cost(op.get_id()) /
                    static_cast<value_t>(num_affected[op.get_id()]);
            }
        }

        auto uniform_cost_function = std::make_shared<
            extra_tasks::VectorProbabilisticOperatorCostFunction>(
            std::move(costs));

        const auto adapted = replace(task, uniform_cost_function);
        const BlindHeuristic<StateRank> h(
            operators,
            *uniform_cost_function,
            term_costs);

        auto& pdb = pdbs.emplace_back(variables, pattern);
        const StateRank init_rank = pdb.get_abstract_state(initial_state);
        compute_distances(pdb, adapted, init_rank, h);
    }

    return std::make_unique<AdditivePDBHeuristic>(std::move(pdbs));
}

} // namespace probfd::heuristics
