#include "downward/cli/search_algorithms/lazy_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/search_algorithms/lazy_search.h"
#include "downward/search_algorithms/search_common.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::lazy_search;
using namespace downward::cli::plugins;

using downward::cli::add_search_algorithm_options_to_feature;
using downward::cli::add_successors_order_options_to_feature;

namespace {
class LazySearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    OperatorCost cost_type;
    int bound;
    utils::FSeconds max_time;
    const std::string& description;
    utils::Verbosity verbosity;
    shared_ptr<TaskDependentFactory<EdgeOpenList>> open_list_factory;
    bool reopen_closed;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories;
    bool randomize_successors;
    bool preferred_successors_first;
    int random_seed;

public:
    explicit LazySearchFactory(
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        const std::string& description,
        utils::Verbosity verbosity,
        shared_ptr<TaskDependentFactory<EdgeOpenList>> open_list_factory,
        bool reopen_closed,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories,
        bool randomize_successors,
        bool preferred_successors_first,
        int random_seed)
        : cost_type(cost_type)
        , bound(bound)
        , max_time(max_time)
        , description(description)
        , verbosity(verbosity)
        , open_list_factory(std::move(open_list_factory))
        , reopen_closed(reopen_closed)
        , preferred_factories(std::move(preferred_factories))
        , randomize_successors(randomize_successors)
        , preferred_successors_first(preferred_successors_first)
        , random_seed(random_seed)
    {
    }

    unique_ptr<SearchAlgorithm>
    create_object(const SharedAbstractTask& task) override
    {
        std::vector<std::shared_ptr<Evaluator>> preferred;

        for (auto& preferred_factory : preferred_factories) {
            preferred.emplace_back(preferred_factory->create_object(task));
        }

        return std::make_unique<LazySearch>(
            open_list_factory->create_object(task),
            reopen_closed,
            std::move(preferred),
            randomize_successors,
            preferred_successors_first,
            random_seed,
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

class LazySearchFeature
    : public SharedTypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          OperatorCost,
          int,
          utils::FSeconds,
          const std::string&,
          utils::Verbosity,
          shared_ptr<TaskDependentFactory<EdgeOpenList>>,
          bool,
          vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
          bool,
          bool,
          int> {
public:
    LazySearchFeature()
        : TypedFeature("lazy", &LazySearchFeature::func)
    {
        document_title("Lazy best-first search");
        document_synopsis("");

        make_required_argument(0, "open", "open list");
        make_optional_argument_with_default(
            1,
            "reopen_closed",
            "false",
            "reopen closed nodes");
        make_optional_argument_with_default(
            2,
            "preferred",
            "[]",
            "use preferred operators of these evaluators");
        const auto n = add_successors_order_options_to_feature(*this, 3);
        add_search_algorithm_options_to_feature(*this, "lazy", n + 3);
    }

    static shared_ptr<TaskDependentFactory<SearchAlgorithm>> func(
        const utils::Context&,
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        const std::string& description,
        utils::Verbosity verbosity,
        shared_ptr<TaskDependentFactory<EdgeOpenList>> open_list_factory,
        bool reopen_closed,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories,
        bool randomize_successors,
        bool preferred_successors_first,
        int random_seed)
    {
        return make_shared_from_arg_tuples<LazySearchFactory>(
            cost_type,
            bound,
            max_time,
            description,
            verbosity,
            std::move(open_list_factory),
            reopen_closed,
            std::move(preferred_factories),
            randomize_successors,
            preferred_successors_first,
            random_seed);
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_lazy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<LazySearchFeature>();
}

} // namespace downward::cli::search_algorithms
