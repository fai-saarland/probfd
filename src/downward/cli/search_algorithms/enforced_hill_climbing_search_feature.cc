#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithm_options.h"

#include "downward/search_algorithms/enforced_hill_climbing_search.h"

#include "downward/utils/logging.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::enforced_hill_climbing_search;

using namespace downward::cli::plugins;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::get_search_algorithm_arguments_from_options;

namespace {

class EnforcedHillClimbingSearchFactory
    : public TaskDependentFactory<SearchAlgorithm> {
    shared_ptr<TaskDependentFactory<Evaluator>> h_factory;
    PreferredUsage preferred_usage;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories;
    OperatorCost cost_type;
    int bound;
    double max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit EnforcedHillClimbingSearchFactory(
        shared_ptr<TaskDependentFactory<Evaluator>> h_factory,
        PreferredUsage preferred_usage,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories,
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string& description,
        utils::Verbosity verbosity)
        : h_factory(std::move(h_factory))
        , preferred_usage(preferred_usage)
        , preferred_factories(std::move(preferred_factories))
        , cost_type(cost_type)
        , bound(bound)
        , max_time(max_time)
        , description(description)
        , verbosity(verbosity)
    {
    }

    unique_ptr<SearchAlgorithm>
    create_object(const SharedAbstractTask& task) override
    {
        std::vector<std::shared_ptr<Evaluator>> preferred;

        for (auto& preferred_factory : preferred_factories) {
            preferred.emplace_back(preferred_factory->create_object(task));
        }

        return std::make_unique<EnforcedHillClimbingSearch>(
            h_factory->create_object(task),
            preferred_usage,
            std::move(preferred),
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

class EnforcedHillClimbingSearchFeature
    : public TypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          EnforcedHillClimbingSearchFactory> {
public:
    EnforcedHillClimbingSearchFeature()
        : TypedFeature("ehc")
    {
        document_title("Lazy enforced hill-climbing");
        document_synopsis("");

        add_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "h",
            "heuristic");
        add_option<PreferredUsage>(
            "preferred_usage",
            "preferred operator usage",
            "prune_by_preferred");
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_search_algorithm_options_to_feature(*this, "ehc");
    }

    shared_ptr<EnforcedHillClimbingSearchFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<EnforcedHillClimbingSearchFactory>(
            opts.get<shared_ptr<TaskDependentFactory<Evaluator>>>("h"),
            opts.get<PreferredUsage>("preferred_usage"),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            get_search_algorithm_arguments_from_options(opts));
    }
};

FeaturePlugin<EnforcedHillClimbingSearchFeature> _plugin;

TypedEnumPlugin<PreferredUsage> _enum_plugin(
    {{"prune_by_preferred",
      "prune successors achieved by non-preferred operators"},
     {"rank_preferred_first",
      "first insert successors achieved by preferred operators, "
      "then those by non-preferred operators"}});

} // namespace
