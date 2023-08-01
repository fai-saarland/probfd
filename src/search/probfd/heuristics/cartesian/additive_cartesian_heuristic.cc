#include "probfd/heuristics/cartesian/additive_cartesian_heuristic.h"

#include "probfd/heuristics/cartesian/cartesian_heuristic_function.h"
#include "probfd/heuristics/cartesian/cost_saturation.h"
#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/types.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

static vector<CartesianHeuristicFunction>
generate_heuristic_functions(const plugins::Options& opts, utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Initializing additive Cartesian heuristic..." << endl;
    }

    CostSaturation cost_saturation(
        opts.get_list<shared_ptr<SubtaskGenerator>>("subtasks"),
        opts.get<shared_ptr<FlawGeneratorFactory>>("flaw_generator_factory"),
        opts.get<shared_ptr<SplitSelectorFactory>>("split_selector_factory"),
        opts.get<int>("max_states"),
        opts.get<int>("max_transitions"),
        opts.get<double>("max_time"),
        opts.get<bool>("use_general_costs"),
        log);

    return cost_saturation.generate_heuristic_functions(
        opts.get<shared_ptr<ProbabilisticTask>>("transform"));
}

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    const plugins::Options& opts)
    : TaskDependentHeuristic(opts)
    , heuristic_functions(generate_heuristic_functions(opts, log))
{
}

EvaluationResult
AdditiveCartesianHeuristic::evaluate(const State& ancestor_state) const
{
    State state = task_proxy.convert_ancestor_state(ancestor_state);
    value_t sum_h = 0;
    for (const CartesianHeuristicFunction& function : heuristic_functions) {
        value_t value = function.get_value(state);
        assert(value >= 0_vt);
        if (value == INFINITE_VALUE)
            return EvaluationResult(true, INFINITE_VALUE);
        sum_h += value;
    }
    assert(sum_h >= 0_vt);
    return EvaluationResult(false, sum_h);
}

class AdditiveCartesianHeuristicFeature
    : public plugins::TypedFeature<TaskEvaluator, AdditiveCartesianHeuristic> {
public:
    AdditiveCartesianHeuristicFeature()
        : TypedFeature("pcegar")
    {
        document_title("Additive CEGAR heuristic");

        add_list_option<shared_ptr<SubtaskGenerator>>(
            "subtasks",
            "subtask generators",
            "[pcegar_landmarks(), pcegar_goals()]");
        add_option<shared_ptr<FlawGeneratorFactory>>(
            "flaw_generator_factory",
            "factory for the flaw generation algorithm used in the refinement "
            "loop",
            "flaws_ilao()");
        add_option<shared_ptr<SplitSelectorFactory>>(
            "split_selector_factory",
            "factory for the split selection algorithm used in the refinement "
            "loop",
            "max_refined()");
        add_option<int>(
            "max_states",
            "maximum sum of abstract states over all abstractions",
            "infinity",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "max_transitions",
            "maximum sum of real transitions (excluding self-loops) over "
            " all abstractions",
            "1M",
            plugins::Bounds("0", "infinity"));
        add_option<double>(
            "max_time",
            "maximum time in seconds for building abstractions",
            "infinity",
            plugins::Bounds("0.0", "infinity"));
        add_option<bool>(
            "use_general_costs",
            "allow negative costs in cost partitioning",
            "true");
        TaskDependentHeuristic::add_options_to_feature(*this);

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }
};

static plugins::FeaturePlugin<AdditiveCartesianHeuristicFeature> _plugin;

} // namespace cartesian
} // namespace heuristics
} // namespace probfd