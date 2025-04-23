#include "probfd/heuristics/ucp_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"

#include "probfd/tasks/modified_operator_costs_task.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/task_utils/task_properties.h"

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

std::unique_ptr<FDREvaluator>
UCPHeuristicFactory::create_heuristic(std::shared_ptr<ProbabilisticTask> task)
{
    const auto pattern_collection_info =
        pattern_collection_generator_->generate(task);

    const auto& patterns = pattern_collection_info.get_patterns();

    const size_t num_abstractions = patterns.size();

    std::vector<ProbabilityAwarePatternDatabase> pdbs;
    pdbs.reserve(num_abstractions);

    std::vector<value_t> costs;
    costs.reserve(task->get_num_operators());

    for (const ProbabilisticOperatorProxy op : task->get_operators()) {
        costs.push_back(
            task->get_operator_cost(op.get_id()) /
            static_cast<value_t>(num_abstractions));
    }

    auto adapted = std::make_shared<extra_tasks::ModifiedOperatorCostsTask>(
        task,
        std::move(costs));

    const State& initial_state = task->get_initial_state();

    const BlindEvaluator<StateRank> h(task->get_operators(), *task, *task);

    for (const Pattern& pattern : patterns) {
        auto& pdb = pdbs.emplace_back(task->get_variables(), pattern);
        const StateRank init_rank = pdb.get_abstract_state(initial_state);
        compute_distances(pdb, adapted, init_rank, h);
    }

    return std::make_unique<UCPHeuristic>(
        task->get_non_goal_termination_cost(),
        std::move(pdbs));
}

} // namespace probfd::heuristics
