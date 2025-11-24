#include "downward/cli/cartesian_abstractions/subtask_generators_features.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/task_dependent_factory.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace downward::cartesian_abstractions;
using namespace downward::utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

void add_task_duplicator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "original",
        &plugins::make_shared<SubtaskGenerator, TaskDuplicator, int>);
    f.make_optional_argument_with_default(
        0,
        "copies",
        "1",
        "number of task copies");
};

void add_goal_decomposition_feature(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "goals",
        &plugins::make_shared<
            SubtaskGenerator,
            GoalDecomposition,
            FactOrder,
            std::shared_ptr<RandomNumberGenerator>>);
    f.make_optional_argument_with_default(
        0,
        "order",
        "hadd_down",
        "ordering of goal or landmark facts");

    add_rng_options_to_feature(f, 1);
}

void add_landmark_decomposition_feature(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "landmarks",
        &plugins::make_shared<
            SubtaskGenerator,
            LandmarkDecomposition,
            std::shared_ptr<TaskDependentFactory<MutexInformation>>,
            FactOrder,
            std::shared_ptr<RandomNumberGenerator>,
            bool>);
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
}

} // namespace

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_task_duplicator_feature(n);
    add_goal_decomposition_feature(n);
    add_landmark_decomposition_feature(n);
}

} // namespace downward::cli::cartesian_abstractions
