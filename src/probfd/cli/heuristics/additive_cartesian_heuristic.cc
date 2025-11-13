#include "probfd/cli/heuristics/additive_cartesian_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

        add_optional_list_argument_with_default<
            std::shared_ptr<SubtaskGenerator>>(
            "subtasks",
            "[pcegar_landmarks(), pcegar_goals()]",
            "subtask generators");
        add_optional_argument_with_default<
            std::shared_ptr<FlawGeneratorFactory>>(
            "flaw_generator_factory",
            "flaws_ilao()",
            "factory for the flaw generation algorithm used in the refinement "
            "loop");
        add_optional_argument_with_default<
            std::shared_ptr<SplitSelectorFactory>>(
            "split_selector_factory",
            "max_refined()",
            "factory for the split selection algorithm used in the refinement "
            "loop");
        add_optional_argument_with_default<int>(
            "max_states",
            "infinity()",
            "maximum sum of abstract states over all abstractions");
        add_optional_argument_with_default<int>(
            "max_transitions",
            "1M",
            "maximum sum of real transitions (excluding self-loops) over "
            " all abstractions");
        add_optional_argument_with_default<downward::utils::FSeconds>(
            "max_time",
            "seconds_max()",
            "maximum time in seconds for building abstractions");
        add_optional_argument_with_default<bool>(
            "use_general_costs",
            "true",
            "allow negative costs in cost partitioning");
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<AdditiveCartesianHeuristicFactory>(
            opts.get_list<std::shared_ptr<SubtaskGenerator>>("subtasks"),
            opts.get_shared<FlawGeneratorFactory>("flaw_generator_factory"),
            opts.get_shared<SplitSelectorFactory>("split_selector_factory"),
            opts.get<int>("max_states"),
            opts.get<int>("max_transitions"),
            opts.get<utils::FSeconds>("max_time"),
            opts.get<bool>("use_general_costs"),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_additive_cartesian_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<AdditiveCartesianHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
