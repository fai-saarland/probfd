#include "downward/cli/plugins/plugin.h"

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

void add_fact_order_options(Feature& feature)
{
    feature.add_option<FactOrder>(
        "order",
        "ordering of goal or landmark facts",
        "hadd_down");
    add_rng_options_to_feature(feature);
}

auto get_fact_order_arguments_from_options(const Options& opts)
{
    return std::tuple_cat(
        std::make_tuple(opts.get<FactOrder>("order")),
        get_rng_arguments_from_options(opts));
}

class TaskDuplicatorFeature
    : public TypedFeature<SubtaskGenerator, TaskDuplicator> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("pcegar_original")
    {
        add_option<int>(
            "copies",
            "number of task copies",
            "1",
            Bounds("1", "infinity"));
    }

    std::shared_ptr<TaskDuplicator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<TaskDuplicator>(
            opts.get<int>("copies"));
    }
};

class GoalDecompositionFeature
    : public TypedFeature<SubtaskGenerator, GoalDecomposition> {
public:
    GoalDecompositionFeature()
        : TypedFeature("pcegar_goals")
    {
        add_fact_order_options(*this);
    }

    std::shared_ptr<GoalDecomposition>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GoalDecomposition>(
            get_fact_order_arguments_from_options(opts));
    }
};

class LandmarkDecompositionFeature
    : public TypedFeature<SubtaskGenerator, LandmarkDecomposition> {
public:
    LandmarkDecompositionFeature()
        : TypedFeature("pcegar_landmarks")
    {
        add_option<std::shared_ptr<TaskDependentFactory<MutexInformation>>>(
            "mutexes",
            "factory for mutexes",
            "mutexes_from_file(\"output.mutexes\")");
        add_fact_order_options(*this);
        add_option<bool>(
            "combine_facts",
            "combine landmark facts with domain abstraction",
            "true");
    }

    std::shared_ptr<LandmarkDecomposition>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            opts.get<std::shared_ptr<TaskDependentFactory<MutexInformation>>>(
                "mutexes"),
            get_fact_order_arguments_from_options(opts),
            opts.get<bool>("combine_facts"));
    }
};

class SubtaskGeneratorCategoryPlugin
    : public TypedCategoryPlugin<SubtaskGenerator> {
public:
    SubtaskGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PSubtaskGenerator")
    {
        document_synopsis("Subtask generator (used by the CEGAR heuristic).");
    }
} _category_plugin;

FeaturePlugin<TaskDuplicatorFeature> _plugin_original;
FeaturePlugin<GoalDecompositionFeature> _plugin_goals;
FeaturePlugin<LandmarkDecompositionFeature> _plugin_landmarks;

TypedEnumPlugin<FactOrder> _enum_plugin(
    {{"original", "according to their (internal) variable index"},
     {"random", "according to a random permutation"},
     {"hadd_up", "according to their h^add value, lowest first"},
     {"hadd_down", "according to their h^add value, highest first "}});

} // namespace
