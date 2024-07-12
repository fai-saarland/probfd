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

namespace {

class AdditiveCartesianHeuristicFactory : public TaskEvaluatorFactory {
    const std::vector<std::shared_ptr<SubtaskGenerator>> subtask_generators;
    const std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory;
    const std::shared_ptr<SplitSelectorFactory> split_selector_factory;
    const int max_states;
    const int max_transitions;
    const double max_time;
    const bool use_general_costs;

    const utils::LogProxy log_;

public:
    AdditiveCartesianHeuristicFactory(
        std::vector<shared_ptr<SubtaskGenerator>> subtasks,
        shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        shared_ptr<SplitSelectorFactory> split_selector_factory,
        int max_states,
        int max_transitions,
        double max_time,
        bool use_general_costs,
        utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

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

class AdditiveCartesianHeuristicFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          AdditiveCartesianHeuristicFactory> {
public:
    AdditiveCartesianHeuristicFactoryFeature()
        : TypedFeature("pcegar")
    {
        document_title("Additive CEGAR heuristic");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");

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
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<AdditiveCartesianHeuristicFactory>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            AdditiveCartesianHeuristicFactory>(
            opts.get_list<shared_ptr<SubtaskGenerator>>("subtasks"),
            opts.get<shared_ptr<FlawGeneratorFactory>>(
                "flaw_generator_factory"),
            opts.get<shared_ptr<SplitSelectorFactory>>(
                "split_selector_factory"),
            opts.get<int>("max_states"),
            opts.get<int>("max_transitions"),
            opts.get<double>("max_time"),
            opts.get<bool>("use_general_costs"),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};

} // namespace

static plugins::FeaturePlugin<AdditiveCartesianHeuristicFactoryFeature> _plugin;

} // namespace probfd::heuristics
