#include "probfd/heuristics/additive_cartesian_heuristic.h"

#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"
#include "probfd/cartesian_abstractions/cost_saturation.h"
#include "probfd/cartesian_abstractions/types.h"
#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/task_evaluator_factory.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;
using namespace probfd::cartesian_abstractions;

namespace probfd::heuristics {

static vector<CartesianHeuristicFunction> generate_heuristic_functions(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators,
    std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
    std::shared_ptr<SplitSelectorFactory> split_selector_factory,
    int max_states,
    int max_transitions,
    double max_time,
    bool use_general_costs)
{
    if (log.is_at_least_normal()) {
        log << "Initializing additive Cartesian heuristic..." << endl;
    }

    CostSaturation cost_saturation(
        subtask_generators,
        std::move(flaw_generator_factory),
        std::move(split_selector_factory),
        max_states,
        max_transitions,
        max_time,
        use_general_costs,
        log);

    return cost_saturation.generate_heuristic_functions(task);
}

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log,
    std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators,
    std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
    std::shared_ptr<SplitSelectorFactory> split_selector_factory,
    int max_states,
    int max_transitions,
    double max_time,
    bool use_general_costs)
    : TaskDependentHeuristic(std::move(task), std::move(log))
    , heuristic_functions_(generate_heuristic_functions(
          this->task_,
          this->log_,
          std::move(subtask_generators),
          std::move(flaw_generator_factory),
          std::move(split_selector_factory),
          max_states,
          max_transitions,
          max_time,
          use_general_costs))
{
}

value_t AdditiveCartesianHeuristic::evaluate(const State& ancestor_state) const
{
    const State state = task_proxy_.convert_ancestor_state(ancestor_state);

    value_t sum_h = 0;
    for (const CartesianHeuristicFunction& function : heuristic_functions_) {
        const value_t value = function.get_value(state);
        assert(value >= 0_vt);
        if (value == INFINITE_VALUE) return INFINITE_VALUE;
        sum_h += value;
    }

    assert(sum_h >= 0_vt);
    return sum_h;
}

AdditiveCartesianHeuristicFactory::AdditiveCartesianHeuristicFactory(
    std::vector<shared_ptr<SubtaskGenerator>> subtasks,
    shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
    shared_ptr<SplitSelectorFactory> split_selector_factory,
    int max_states,
    int max_transitions,
    double max_time,
    bool use_general_costs,
    utils::Verbosity verbosity)
    : subtask_generators(std::move(subtasks))
    , flaw_generator_factory(std::move(flaw_generator_factory))
    , split_selector_factory(std::move(split_selector_factory))
    , max_states(max_states)
    , max_transitions(max_transitions)
    , max_time(max_time)
    , use_general_costs(use_general_costs)
    , log_(utils::get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDREvaluator>
AdditiveCartesianHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction>)
{
    return std::make_unique<AdditiveCartesianHeuristic>(
        task,
        log_,
        subtask_generators,
        flaw_generator_factory,
        split_selector_factory,
        max_states,
        max_transitions,
        max_time,
        use_general_costs);
}

} // namespace probfd::heuristics
