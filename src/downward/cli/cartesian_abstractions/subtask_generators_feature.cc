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

class TaskDuplicatorFeature : public SharedTypedFeature<SubtaskGenerator, int> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("original", &TaskDuplicatorFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "copies",
            "1",
            "number of task copies");
    }

    static shared_ptr<SubtaskGenerator> func(const Context&, int copies)
    {
        return make_shared_from_arg_tuples<TaskDuplicator>(copies);
    }
};

class GoalDecompositionFeature
    : public SharedTypedFeature<SubtaskGenerator, FactOrder, int> {
public:
    GoalDecompositionFeature()
        : TypedFeature("goals", &GoalDecompositionFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "order",
            "hadd_down",
            "ordering of goal or landmark facts");

        add_rng_options_to_feature(*this, 1);
    }

    static shared_ptr<SubtaskGenerator>
    func(const Context&, FactOrder order, int random_seed)
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
          int,
          bool> {
public:
    LandmarkDecompositionFeature()
        : TypedFeature("landmarks", &LandmarkDecompositionFeature::func)
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

    static shared_ptr<SubtaskGenerator> func(
        const Context&,
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        FactOrder order,
        int random_seed,
        bool combine_facts)
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            std::move(mutex_factory),
            order,
            random_seed,
            combine_facts);
    }
};
} // namespace

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<TaskDuplicatorFeature>();
    n.insert_feature_plugin<GoalDecompositionFeature>();
    n.insert_feature_plugin<LandmarkDecompositionFeature>();
}

} // namespace downward::cli::cartesian_abstractions
