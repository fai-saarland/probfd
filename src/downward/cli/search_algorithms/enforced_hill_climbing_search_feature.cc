#include "downward/cli/search_algorithms/enforced_hill_climbing_search_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

namespace {
class EnforcedHillClimbingSearchFactory
    : public TaskDependentFactory<SearchAlgorithm> {
    shared_ptr<TaskDependentFactory<Evaluator>> h_factory;
    PreferredUsage preferred_usage;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories;
    OperatorCost cost_type;
    int bound;
    utils::FSeconds max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit EnforcedHillClimbingSearchFactory(
        shared_ptr<TaskDependentFactory<Evaluator>> h_factory,
        PreferredUsage preferred_usage,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories,
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
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

Feature& add_eager_hill_climbing_search_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "ehc",
        &cli::plugins::make_shared<
            TaskDependentFactory<SearchAlgorithm>,
            EnforcedHillClimbingSearchFactory,
            shared_ptr<TaskDependentFactory<Evaluator>>,
            PreferredUsage,
            vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
            OperatorCost,
            int,
            FSeconds,
            const std::string&,
            Verbosity>);

    f.document_title("Lazy enforced hill-climbing");
    f.document_synopsis("");

    f.make_required_argument(0, "h", "heuristic");
    f.make_optional_argument_with_default(
        1,
        "preferred_usage",
        "prune_by_preferred",
        "preferred operator usage");
    f.make_optional_argument_with_default(
        2,
        "preferred",
        "[]",
        "use preferred operators of these evaluators");
    add_search_algorithm_options_to_feature(f, "ehc", 3);

    return f;
}

} // namespace

namespace downward::cli::search_algorithms {

void add_enforce_hill_climbing_search_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<PreferredUsage>(
        {{"prune_by_preferred",
          "prune successors achieved by non-preferred operators"},
         {"rank_preferred_first",
          "first insert successors achieved by preferred operators, "
          "then those by non-preferred operators"}});

    add_eager_hill_climbing_search_to_namespace(n);
}

} // namespace downward::cli::search_algorithms
