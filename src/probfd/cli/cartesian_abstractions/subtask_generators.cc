#include "probfd/cli/cartesian_abstractions/subtask_generators.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cartesian_abstractions/subtask_generators.h"

using namespace downward;
using namespace utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase& add_task_duplicator_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pcegar_original",
        &cli::plugins::construct_shared<SubtaskGenerator, TaskDuplicator, int>);

    f.make_optional_argument_with_default(
        0,
        "copies",
        "1",
        "number of task copies");

    return f;
}

InternalFunctionDefinitionBase& add_goal_decomposition_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pcegar_goals",
        &cli::plugins::
            construct_shared<SubtaskGenerator, GoalDecomposition, FactOrder, std::shared_ptr<RandomNumberGenerator>>);

    f.make_optional_argument_with_default(
        0,
        "order",
        "hadd_down",
        "ordering of goal or landmark facts");
    add_rng_options_to_feature(f, 1);

    return f;
}

InternalFunctionDefinitionBase& add_landmark_decomposition_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pcegar_landmarks",
        &cli::plugins::construct_shared<
            SubtaskGenerator,
            LandmarkDecomposition,
            std::shared_ptr<TaskDependentFactory<MutexInformation>>,
            FactOrder,
            bool,
            std::shared_ptr<RandomNumberGenerator>>);

    f.make_optional_argument_with_default(
        0,
        "mutexes",
        "mutexes_from_file(\"output.mutexes\")",
        "factory for mutexes");

    f.make_optional_argument_with_default(
        1,
        "order",
        "hadd_down",
        "ordering of goal or landmark facts");

    const auto n = add_rng_options_to_feature(f, 2);

    f.make_optional_argument_with_default(
        n + 2,
        "combine_facts",
        "true",
        "combine landmark facts with domain abstraction");

    return f;
}

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<SubtaskGenerator>(
        "PSubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");
}

void add_subtask_generator_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});

    add_task_duplicator_to_namespace(n);
    add_goal_decomposition_to_namespace(n);
    add_landmark_decomposition_to_namespace(n);
}

} // namespace probfd::cli::cartesian_abstractions
