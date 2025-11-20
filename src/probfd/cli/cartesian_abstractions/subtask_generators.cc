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
class TaskDuplicatorFeature : public SharedTypedFeature<SubtaskGenerator, int> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("pcegar_original", &TaskDuplicatorFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "copies",
            "1",
            "number of task copies");
    }

    static std::shared_ptr<SubtaskGenerator> func(int copies)
    {
        return make_shared_from_arg_tuples<TaskDuplicator>(copies);
    }
};

class GoalDecompositionFeature
    : public SharedTypedFeature<SubtaskGenerator, FactOrder, int> {
public:
    GoalDecompositionFeature()
        : TypedFeature("pcegar_goals", &GoalDecompositionFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "order",
            "hadd_down",
            "ordering of goal or landmark facts");
        add_rng_options_to_feature(*this, 1);
    }

    static std::shared_ptr<SubtaskGenerator>
    func(FactOrder order, int random_seed)
    {
        return make_shared_from_arg_tuples<GoalDecomposition>(
            order,
            random_seed);
    }
};

class LandmarkDecompositionFeature
    : public SharedTypedFeature<
          SubtaskGenerator,
          std::shared_ptr<TaskDependentFactory<MutexInformation>>,
          FactOrder,
          bool,
          int> {
public:
    LandmarkDecompositionFeature()
        : TypedFeature("pcegar_landmarks", &LandmarkDecompositionFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "mutexes",
            "mutexes_from_file(\"output.mutexes\")",
            "factory for mutexes");

        make_optional_argument_with_default(
            1,
            "order",
            "hadd_down",
            "ordering of goal or landmark facts");

        const auto n = add_rng_options_to_feature(*this, 2);

        make_optional_argument_with_default(
            n + 2,
            "combine_facts",
            "true",
            "combine landmark facts with domain abstraction");
    }

    static std::shared_ptr<SubtaskGenerator> func(
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        FactOrder order,
        bool combine_facts,
        int random_seed)
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            std::move(mutex_factory),
            order,
            combine_facts,
            random_seed);
    }
};

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_category_plugin<SubtaskGenerator>(
        "PSubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");
}

void add_subtask_generator_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});

    n.insert_feature_plugin<TaskDuplicatorFeature>();
    n.insert_feature_plugin<GoalDecompositionFeature>();
    n.insert_feature_plugin<LandmarkDecompositionFeature>();
}

} // namespace probfd::cli::cartesian_abstractions
