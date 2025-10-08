#include "downward/cli/search_algorithms/lazy_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
using downward::cli::get_search_algorithm_arguments_from_options;
using downward::cli::get_successors_order_arguments_from_options;

namespace {
class LazySearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    OperatorCost cost_type;
    int bound;
    utils::Duration max_time;
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
        utils::Duration max_time,
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
    : public SharedTypedFeature<TaskDependentFactory<SearchAlgorithm>> {
public:
    LazySearchFeature()
        : SharedTypedFeature("lazy")
    {
        document_title("Lazy best-first search");
        document_synopsis("");

        add_option<shared_ptr<TaskDependentFactory<EdgeOpenList>>>(
            "open",
            "open list");
        add_option<bool>("reopen_closed", "reopen closed nodes", "false");
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_successors_order_options_to_feature(*this);
        add_search_algorithm_options_to_feature(*this, "lazy");
    }

    shared_ptr<TaskDependentFactory<SearchAlgorithm>>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<LazySearchFactory>(
            get_search_algorithm_arguments_from_options(opts),
            opts.get<shared_ptr<TaskDependentFactory<EdgeOpenList>>>("open"),
            opts.get<bool>("reopen_closed"),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            get_successors_order_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_lazy_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LazySearchFeature>();
}

} // namespace downward::cli::search_algorithms
