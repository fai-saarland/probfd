#include "probfd/cli/cartesian_abstractions/subtask_generators.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cartesian_abstractions/subtask_generators.h"

using namespace downward;
using namespace utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

using namespace probfd::cartesian_abstractions;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class TaskDuplicatorFeature : public SharedTypedFeature<SubtaskGenerator> {
public:
    TaskDuplicatorFeature()
        : SharedTypedFeature("pcegar_original")
    {
        add_option<int>(
            "copies",
            "number of task copies",
            "1",
            Bounds("1", "infinity"));
    }

    std::shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<TaskDuplicator>(
            opts.get<int>("copies"));
    }
};

class GoalDecompositionFeature : public SharedTypedFeature<SubtaskGenerator> {
public:
    GoalDecompositionFeature()
        : SharedTypedFeature("pcegar_goals")
    {
        add_option<FactOrder>(
            "order",
            "ordering of goal or landmark facts",
            "hadd_down");
        add_rng_options_to_feature(*this);
    }

    std::shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GoalDecomposition>(
            opts.get<FactOrder>("order"),
            get_rng_arguments_from_options(opts));
    }
};

class LandmarkDecompositionFeature : public SharedTypedFeature<SubtaskGenerator> {
public:
    LandmarkDecompositionFeature()
        : SharedTypedFeature("pcegar_landmarks")
    {
        add_option<std::shared_ptr<TaskDependentFactory<MutexInformation>>>(
            "mutexes",
            "factory for mutexes",
            "mutexes_from_file(\"output.mutexes\")");
        add_option<FactOrder>(
            "order",
            "ordering of goal or landmark facts",
            "hadd_down");
        add_rng_options_to_feature(*this);
        add_option<bool>(
            "combine_facts",
            "combine landmark facts with domain abstraction",
            "true");
    }

    std::shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            opts.get_shared<TaskDependentFactory<MutexInformation>>(
                "mutexes"),
            opts.get<FactOrder>("order"),
            opts.get<bool>("combine_facts"),
            get_rng_arguments_from_options(opts));
    }
};

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_features(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<SubtaskGenerator>(
        "PSubtaskGenerator");
    category.document_synopsis(
        "Subtask generator (used by the CEGAR heuristic).");

    raw_registry.insert_feature_plugin<TaskDuplicatorFeature>();
    raw_registry.insert_feature_plugin<GoalDecompositionFeature>();
    raw_registry.insert_feature_plugin<LandmarkDecompositionFeature>();

    raw_registry.insert_enum_plugin<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});
}

} // namespace probfd::cli::cartesian_abstractions
