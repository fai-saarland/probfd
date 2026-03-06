#include "probfd/cli/cartesian_abstractions/subtask_generators.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

using namespace downward;
using namespace utils;

using namespace downward::cli;
using namespace language::parser;

using namespace probfd::cartesian_abstractions;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_task_duplicator_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<
        &language::parser::
            construct_shared<SubtaskGenerator, TaskDuplicator, int>>(
        nspace,
        "pcegar_original");

    f.make_optional_argument_with_default(
        0,
        "copies",
        "1",
        "number of task copies");

    return f;
}

InternalFunctionDefinitionBase&
add_goal_decomposition_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SubtaskGenerator,
        GoalDecomposition,
        FactOrder,
        std::shared_ptr<RandomNumberGenerator>>>(nspace, "pcegar_goals");

    f.make_optional_argument_with_default(
        0,
        "order",
        "hadd_down",
        "ordering of goal or landmark facts");
    add_rng_options_to_feature(f, 1);

    return f;
}

InternalFunctionDefinitionBase&
add_landmark_decomposition_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        SubtaskGenerator,
        LandmarkDecomposition,
        std::shared_ptr<TaskDependentFactory<MutexInformation>>,
        FactOrder,
        bool,
        std::shared_ptr<RandomNumberGenerator>>>(nspace, "pcegar_landmarks");

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

void add_subtask_generator_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<SubtaskGenerator>(
        nspace,
        "PSubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");
}

void add_subtask_generator_features(NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<FactOrder>(
        nspace,
        "FactOrder",
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});

    add_task_duplicator_to_namespace(nspace);
    add_goal_decomposition_to_namespace(nspace);
    add_landmark_decomposition_to_namespace(nspace);
}

} // namespace probfd::cli::cartesian_abstractions
