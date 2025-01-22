#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cartesian_abstractions/subtask_generators.h"
#include "downward/cartesian_abstractions/utils.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/landmarks/landmark_graph.h"

#include "downward/task_utils/task_properties.h"

#include "downward/tasks/modified_goals_task.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"

using namespace std;
using namespace downward;
using namespace downward::cartesian_abstractions;
using namespace downward::utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

void add_fact_order_option(Feature& feature)
{
    feature.add_option<FactOrder>(
        "order",
        "ordering of goal or landmark facts",
        "hadd_down");
    add_rng_options_to_feature(feature);
}

tuple<FactOrder, int> get_fact_order_arguments_from_options(const Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get<FactOrder>("order")),
        get_rng_arguments_from_options(opts));
}

class TaskDuplicatorFeature
    : public TypedFeature<SubtaskGenerator, TaskDuplicator> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("original")
    {
        add_option<int>(
            "copies",
            "number of task copies",
            "1",
            Bounds("1", "infinity"));
    }

    virtual shared_ptr<TaskDuplicator>
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
        : TypedFeature("goals")
    {
        add_fact_order_option(*this);
    }

    virtual shared_ptr<GoalDecomposition>
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
        : TypedFeature("landmarks")
    {
        add_option<std::shared_ptr<MutexFactory>>(
            "mutexes",
            "factory for mutexes",
            "mutexes_from_file(\"output.mutexes\")");
        add_fact_order_option(*this);
        add_option<bool>(
            "combine_facts",
            "combine landmark facts with domain abstraction",
            "true");
    }

    virtual shared_ptr<LandmarkDecomposition>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            opts.get<std::shared_ptr<MutexFactory>>("mutexes"),
            get_fact_order_arguments_from_options(opts),
            opts.get<bool>("combine_facts"));
    }
};

FeaturePlugin<TaskDuplicatorFeature> _plugin_original;
FeaturePlugin<GoalDecompositionFeature> _plugin_goals;
FeaturePlugin<LandmarkDecompositionFeature> _plugin_landmarks;

TypedEnumPlugin<FactOrder> _enum_plugin(
    {{"original", "according to their (internal) variable index"},
     {"random", "according to a random permutation"},
     {"hadd_up", "according to their h^add value, lowest first"},
     {"hadd_down", "according to their h^add value, highest first "}});

} // namespace
