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
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
void add_fact_order_option(Feature& feature)
{
    feature.add_optional_argument_with_default<FactOrder>(
        "order",
        "hadd_down",
        "ordering of goal or landmark facts");
    add_rng_options_to_feature(feature);
}

tuple<FactOrder, int> get_fact_order_arguments_from_options(const Options& opts)
{
    return tuple_cat(
        make_tuple(opts.get<FactOrder>("order")),
        get_rng_arguments_from_options(opts));
}

class TaskDuplicatorFeature : public SharedTypedFeature<SubtaskGenerator> {
public:
    TaskDuplicatorFeature()
        : TypedFeature("original")
    {
        add_optional_argument_with_default<int>(
            "copies",
            "1",
            "number of task copies");
    }

    virtual shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<TaskDuplicator>(
            opts.get<int>("copies"));
    }
};

class GoalDecompositionFeature : public SharedTypedFeature<SubtaskGenerator> {
public:
    GoalDecompositionFeature()
        : TypedFeature("goals")
    {
        add_fact_order_option(*this);
    }

    virtual shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<GoalDecomposition>(
            get_fact_order_arguments_from_options(opts));
    }
};

class LandmarkDecompositionFeature
    : public SharedTypedFeature<SubtaskGenerator> {
public:
    LandmarkDecompositionFeature()
        : TypedFeature("landmarks")
    {
        add_optional_argument_with_default<
            std::shared_ptr<TaskDependentFactory<MutexInformation>>>(
            "mutexes",
            "mutexes_from_file(\"output.mutexes\")",
            "factory for mutexes");
        add_fact_order_option(*this);
        add_optional_argument_with_default<bool>(
            "combine_facts",
            "true",
            "combine landmark facts with domain abstraction");
    }

    virtual shared_ptr<SubtaskGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkDecomposition>(
            opts.get_shared<TaskDependentFactory<MutexInformation>>("mutexes"),
            get_fact_order_arguments_from_options(opts),
            opts.get<bool>("combine_facts"));
    }
};
} // namespace

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(Registry& raw_registry)
{
    raw_registry.insert_feature_plugin<TaskDuplicatorFeature>();
    raw_registry.insert_feature_plugin<GoalDecompositionFeature>();
    raw_registry.insert_feature_plugin<LandmarkDecompositionFeature>();
}

} // namespace downward::cli::cartesian_abstractions
