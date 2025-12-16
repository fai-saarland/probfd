#include "probfd/cli/heuristics/additive_cartesian_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/additive_cartesian_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::cartesian_abstractions;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase&
add_probabilistic_additive_cartesian_cegar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pcegar",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            AdditiveCartesianHeuristicFactory,
            std::vector<std::shared_ptr<SubtaskGenerator>>,
            std::shared_ptr<FlawGeneratorFactory>,
            std::shared_ptr<SplitSelectorFactory>,
            int,
            int,
            utils::FSeconds,
            bool,
            utils::Verbosity>);

    f.document_title("Additive Cartesian CEGAR heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    f.make_optional_argument_with_default(
        0,
        "subtasks",
        "[pcegar_landmarks(), pcegar_goals()]",
        "subtask generators");

    f.make_optional_argument_with_default(
        1,
        "flaw_generator_factory",
        "flaws_ilao()",
        "factory for the flaw generation algorithm used in the refinement "
        "loop");

    f.make_optional_argument_with_default(
        2,
        "split_selector_factory",
        "max_refined()",
        "factory for the split selection algorithm used in the refinement "
        "loop");

    f.make_optional_argument_with_default(
        3,
        "max_states",
        "infinity()",
        "maximum sum of abstract states over all abstractions");

    f.make_optional_argument_with_default(
        4,
        "max_transitions",
        "1M",
        "maximum sum of real transitions (excluding self-loops) over "
        " all abstractions");

    f.make_optional_argument_with_default(
        5,
        "max_time",
        "seconds_max()",
        "maximum time in seconds for building abstractions");

    f.make_optional_argument_with_default(
        6,
        "use_general_costs",
        "true",
        "allow negative costs in cost partitioning");

    add_task_dependent_heuristic_options_to_feature(f, 7);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_additive_cartesian_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_probabilistic_additive_cartesian_cegar_to_namespace(n);
}

} // namespace probfd::cli::heuristics
