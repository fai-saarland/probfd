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
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          std::vector<std::shared_ptr<SubtaskGenerator>>,
          std::shared_ptr<FlawGeneratorFactory>,
          std::shared_ptr<SplitSelectorFactory>,
          int,
          int,
          utils::FSeconds,
          bool,
          utils::Verbosity> {
public:
    AdditiveCartesianHeuristicFactoryFeature()
        : TypedFeature(
              "pcegar",
              &AdditiveCartesianHeuristicFactoryFeature::func)
    {
        document_title("Additive Cartesian CEGAR heuristic");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");

        make_optional_argument_with_default(
            0,
            "subtasks",
            "[pcegar_landmarks(), pcegar_goals()]",
            "subtask generators");

        make_optional_argument_with_default(
            1,
            "flaw_generator_factory",
            "flaws_ilao()",
            "factory for the flaw generation algorithm used in the refinement "
            "loop");

        make_optional_argument_with_default(
            2,
            "split_selector_factory",
            "max_refined()",
            "factory for the split selection algorithm used in the refinement "
            "loop");

        make_optional_argument_with_default(
            3,
            "max_states",
            "infinity()",
            "maximum sum of abstract states over all abstractions");

        make_optional_argument_with_default(
            4,
            "max_transitions",
            "1M",
            "maximum sum of real transitions (excluding self-loops) over "
            " all abstractions");

        make_optional_argument_with_default(
            5,
            "max_time",
            "seconds_max()",
            "maximum time in seconds for building abstractions");

        make_optional_argument_with_default(
            6,
            "use_general_costs",
            "true",
            "allow negative costs in cost partitioning");

        add_task_dependent_heuristic_options_to_feature(*this, 7);
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        std::vector<std::shared_ptr<SubtaskGenerator>> subtasks,
        std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
        std::shared_ptr<SplitSelectorFactory> split_selector_factory,
        int max_states,
        int max_transitions,
        utils::FSeconds max_time,
        bool use_general_costs,
        utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<AdditiveCartesianHeuristicFactory>(
            std::move(subtasks),
            std::move(flaw_generator_factory),
            std::move(split_selector_factory),
            max_states,
            max_transitions,
            max_time,
            use_general_costs,
            verbosity);
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
