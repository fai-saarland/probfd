#include "probfd/heuristics/additive_cartesian_heuristic.h"

#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"
#include "probfd/cartesian_abstractions/cost_saturation.h"
#include "probfd/cartesian_abstractions/types.h"
#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;
using namespace downward;
using namespace probfd::cartesian_abstractions;

namespace probfd::heuristics {

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    std::vector<CartesianHeuristicFunction> heuristic_functions)
    : heuristic_functions_(std::move(heuristic_functions))
{
}

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    AdditiveCartesianHeuristic&&) noexcept = default;

AdditiveCartesianHeuristic::~AdditiveCartesianHeuristic() = default;

value_t AdditiveCartesianHeuristic::evaluate(const State& state) const
{
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
    , log_(get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDREvaluator>
AdditiveCartesianHeuristicFactory::create_heuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction>)
{
    if (log_.is_at_least_normal()) {
        log_ << "Initializing additive Cartesian heuristic..." << endl;
    }

    CostSaturation cost_saturation(
        subtask_generators,
        std::move(flaw_generator_factory),
        std::move(split_selector_factory),
        max_states,
        max_transitions,
        max_time,
        use_general_costs,
        log_);

    return std::make_unique<AdditiveCartesianHeuristic>(
        cost_saturation.generate_heuristic_functions(task));
}

} // namespace probfd::heuristics
