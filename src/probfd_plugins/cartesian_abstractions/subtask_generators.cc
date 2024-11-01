#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

using namespace probfd::cartesian_abstractions;

namespace {

void add_fact_order_options(plugins::Feature& feature)
{
    feature.add_option<FactOrder>(
        "order",
        "ordering of goal or landmark facts",
        "hadd_down");
    utils::add_rng_options_to_feature(feature);
}

auto get_fact_order_arguments_from_options(const plugins::Options& opts)
{
    return std::tuple_cat(
        std::make_tuple(opts.get<FactOrder>("order")),
        utils::get_rng_arguments_from_options(opts));
}

class TaskDuplicatorFeature
    : public plugins::TypedFeature<SubtaskGenerator, TaskDuplicator> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("pcegar_original")
    {
        add_option<int>(
            "copies",
            "number of task copies",
            "1",
            plugins::Bounds("1", "infinity"));
    }

    std::shared_ptr<TaskDuplicator>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<TaskDuplicator>(
            opts.get<int>("copies"));
    }
};

class GoalDecompositionFeature
    : public plugins::TypedFeature<SubtaskGenerator, GoalDecomposition> {
public:
    GoalDecompositionFeature()
        : TypedFeature("pcegar_goals")
    {
        add_fact_order_options(*this);
    }

    std::shared_ptr<GoalDecomposition>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<GoalDecomposition>(
            get_fact_order_arguments_from_options(opts));
    }
};

class LandmarkDecompositionFeature
    : public plugins::TypedFeature<SubtaskGenerator, LandmarkDecomposition> {
public:
    LandmarkDecompositionFeature()
        : TypedFeature("pcegar_landmarks")
    {
        add_fact_order_options(*this);
        add_option<bool>(
            "combine_facts",
            "combine landmark facts with domain abstraction",
            "true");
    }

    std::shared_ptr<LandmarkDecomposition>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<LandmarkDecomposition>(
            get_fact_order_arguments_from_options(opts),
            opts.get<bool>("combine_facts"));
    }
};

class SubtaskGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<SubtaskGenerator> {
public:
    SubtaskGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PSubtaskGenerator")
    {
        document_synopsis("Subtask generator (used by the CEGAR heuristic).");
    }
} _category_plugin;

plugins::FeaturePlugin<TaskDuplicatorFeature> _plugin_original;
plugins::FeaturePlugin<GoalDecompositionFeature> _plugin_goals;
plugins::FeaturePlugin<LandmarkDecompositionFeature> _plugin_landmarks;

plugins::TypedEnumPlugin<FactOrder> _enum_plugin(
    {{"original", "according to their (internal) variable index"},
     {"random", "according to a random permutation"},
     {"hadd_up", "according to their h^add value, lowest first"},
     {"hadd_down", "according to their h^add value, highest first "}});

} // namespace
