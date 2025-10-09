#include "probfd/cli/heuristics/additive_cartesian_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/additive_cartesian_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::cartesian_abstractions;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

namespace {
class AdditiveCartesianHeuristicFactoryFeature
    : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    AdditiveCartesianHeuristicFactoryFeature()
        : SharedTypedFeature("pcegar")
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
        add_duration(
            "max_time",
            "maximum time in seconds for building abstractions",
            "infinite");
        add_option<bool>(
            "use_general_costs",
            "allow negative costs in cost partitioning",
            "true");
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<AdditiveCartesianHeuristicFactory>(
            opts.get_list<std::shared_ptr<SubtaskGenerator>>("subtasks"),
            opts.get_shared<FlawGeneratorFactory>(
                "flaw_generator_factory"),
            opts.get_shared<SplitSelectorFactory>(
                "split_selector_factory"),
            opts.get<int>("max_states"),
            opts.get<int>("max_transitions"),
            opts.get<utils::Duration>("max_time"),
            opts.get<bool>("use_general_costs"),
            get_task_dependent_heuristic_arguments_from_options(opts));
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
