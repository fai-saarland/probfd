#include "probfd/heuristics/cartesian/additive_cartesian_heuristic.h"

#include "probfd/heuristics/cartesian/cartesian_heuristic_function.h"
#include "probfd/heuristics/cartesian/cost_saturation.h"
#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/types.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "plugins/plugin.h"

#include "utils/logging.h"
#include "utils/markup.h"
#include "utils/rng.h"
#include "utils/rng_options.h"

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
        opts.get<int>("max_states"),
        opts.get<int>("max_search_states"),
        opts.get<int>("max_transitions"),
        opts.get<double>("max_time"),
        opts.get<bool>("use_general_costs"),
        opts.get<PickSplit>("pick"),
        *utils::parse_rng_from_options(opts),
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
        add_option<int>(
            "max_states",
            "maximum sum of abstract states over all abstractions",
            "infinity",
            plugins::Bounds("1", "infinity"));
        add_option<int>(
            "max_search_states",
            "maximum number of concrete states allowed to be generated during "
            "flaw "
            "search before giving up",
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
        add_option<PickSplit>(
            "pick",
            "how to choose on which variable to split the flaw state",
            "max_refined");
        add_option<bool>(
            "use_general_costs",
            "allow negative costs in cost partitioning",
            "true");
        TaskDependentHeuristic::add_options_to_feature(*this);
        utils::add_rng_options(*this);

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

static plugins::TypedEnumPlugin<PickSplit> _enum_plugin(
    {{"random", "select a random variable (among all eligible variables)"},
     {"min_unwanted",
      "select an eligible variable which has the least unwanted values "
      "(number of values of v that land in the abstract state whose "
      "h-value will probably be raised) in the flaw state"},
     {"max_unwanted",
      "select an eligible variable which has the most unwanted values "
      "(number of values of v that land in the abstract state whose "
      "h-value will probably be raised) in the flaw state"},
     {"min_refined",
      "select an eligible variable which is the least refined "
      "(-1 * (remaining_values(v) / original_domain_size(v))) "
      "in the flaw state"},
     {"max_refined",
      "select an eligible variable which is the most refined "
      "(-1 * (remaining_values(v) / original_domain_size(v))) "
      "in the flaw state"},
     {"min_hadd",
      "select an eligible variable with minimal h^add(s_0) value "
      "over all facts that need to be removed from the flaw state"},
     {"max_hadd",
      "select an eligible variable with maximal h^add(s_0) value "
      "over all facts that need to be removed from the flaw state"}});

} // namespace cartesian
} // namespace heuristics
} // namespace probfd