#include "probfd/heuristics/cartesian/additive_cartesian_heuristic.h"

#include "probfd/heuristics/cartesian/cartesian_heuristic_function.h"
#include "probfd/heuristics/cartesian/cost_saturation.h"
#include "probfd/heuristics/cartesian/types.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "option_parser.h"
#include "plugin.h"

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
generate_heuristic_functions(const options::Options& opts, utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Initializing additive Cartesian heuristic..." << endl;
    }

    CostSaturation cost_saturation(
        opts.get_list<shared_ptr<SubtaskGenerator>>("subtasks"),
        opts.get<int>("max_states"),
        opts.get<int>("max_transitions"),
        opts.get<double>("max_time"),
        opts.get<PickSplit>("pick"),
        *utils::parse_rng_from_options(opts),
        log);

    return cost_saturation.generate_heuristic_functions(
        opts.get<shared_ptr<ProbabilisticTask>>("transform"));
}

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    const options::Options& opts)
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

static shared_ptr<TaskEvaluator> _parse(OptionParser& parser)
{
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");

    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");

    parser.add_list_option<shared_ptr<SubtaskGenerator>>(
        "subtasks",
        "subtask generators",
        "[pcegar_landmarks(), pcegar_goals()]");
    parser.add_option<int>(
        "max_states",
        "maximum sum of abstract states over all abstractions",
        "infinity",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "max_transitions",
        "maximum sum of real transitions (excluding self-loops) over "
        " all abstractions",
        "1M",
        Bounds("0", "infinity"));
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds for building abstractions",
        "infinity",
        Bounds("0.0", "infinity"));

    vector<string> pick_strategies;
    pick_strategies.push_back("RANDOM");
    pick_strategies.push_back("MIN_UNWANTED");
    pick_strategies.push_back("MAX_UNWANTED");
    pick_strategies.push_back("MIN_REFINED");
    pick_strategies.push_back("MAX_REFINED");
    pick_strategies.push_back("MIN_HADD");
    pick_strategies.push_back("MAX_HADD");
    parser.add_enum_option<PickSplit>(
        "pick",
        pick_strategies,
        "split-selection strategy",
        "MAX_REFINED");

    TaskDependentHeuristic::add_options_to_parser(parser);

    utils::add_rng_options(parser);

    Options opts = parser.parse();

    if (parser.dry_run()) return nullptr;

    return make_shared<AdditiveCartesianHeuristic>(opts);
}

static Plugin<TaskEvaluator> _plugin("pcegar", _parse);

} // namespace cartesian
} // namespace heuristics
} // namespace probfd