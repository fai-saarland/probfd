#include "probfd_cli/heuristics/additive_cartesian_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd_cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/additive_cartesian_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::cartesian_abstractions;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language;
using namespace language::plugins;

namespace {
class AdditiveCartesianHeuristicFactoryFeature
    : public TypedFeature<TaskHeuristicFactory> {
public:
    AdditiveCartesianHeuristicFactoryFeature()
        : TypedFeature("pcegar")
    {
        document_title("Additive Cartesian CEGAR heuristic");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");

        add_list_option<std::shared_ptr<SubtaskGenerator>>(
            "subtasks",
            "subtask generators",
            "[pcegar_landmarks(), pcegar_goals()]");
        add_option<std::shared_ptr<FlawGeneratorFactory>>(
            "flaw_generator_factory",
            "factory for the flaw generation algorithm used in the refinement "
            "loop",
            "flaws_ilao()");
        add_option<std::shared_ptr<SplitSelectorFactory>>(
            "split_selector_factory",
            "factory for the split selection algorithm used in the refinement "
            "loop",
            "max_refined()");
        add_option<int>(
            "max_states",
            "maximum sum of abstract states over all abstractions",
            "infinity",
            Bounds("1", "infinity"));
        add_option<int>(
            "max_transitions",
            "maximum sum of real transitions (excluding self-loops) over "
            " all abstractions",
            "1M",
            Bounds("0", "infinity"));
        add_option<utils::Duration>(
            "max_time",
            "maximum time in seconds for building abstractions",
            "infinity",
            Bounds("0.0", "infinity"));
        add_option<bool>(
            "use_general_costs",
            "allow negative costs in cost partitioning",
            "true");
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<AdditiveCartesianHeuristicFactory>(
            opts.get_list<std::shared_ptr<SubtaskGenerator>>(
                context,
                "subtasks"),
            opts.get<std::shared_ptr<FlawGeneratorFactory>>(
                context,
                "flaw_generator_factory"),
            opts.get<std::shared_ptr<SplitSelectorFactory>>(
                context,
                "split_selector_factory"),
            opts.get<int>(context, "max_states"),
            opts.get<int>(context, "max_transitions"),
            opts.get<utils::Duration>(context, "max_time"),
            opts.get<bool>(context, "use_general_costs"),
            get_task_dependent_heuristic_arguments_from_options(context, opts));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_additive_cartesian_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<AdditiveCartesianHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
