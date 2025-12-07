#include "downward/cli/search_algorithms/lazy_feature.h"
#include "downward/cli/search_algorithms/search_algorithm_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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
    std::shared_ptr<utils::RandomNumberGenerator> rng;

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
        std::shared_ptr<utils::RandomNumberGenerator> rng)
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
        , rng(std::move(rng))
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
            rng,
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

InternalFunctionDefinitionBase& add_lazy_search_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lazy",
        &cli::plugins::construct_shared<
            TaskDependentFactory<SearchAlgorithm>,
            LazySearchFactory,
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
            std::shared_ptr<utils::RandomNumberGenerator>>);

    f.document_title("Lazy best-first search");
    f.document_synopsis("");

    f.make_required_argument(0, "open", "open list");
    f.make_optional_argument_with_default(
        1,
        "reopen_closed",
        "false",
        "reopen closed nodes");
    f.make_optional_argument_with_default(
        2,
        "preferred",
        "[]",
        "use preferred operators of these evaluators");
    const auto n = add_successors_order_options_to_feature(f, 3);
    add_search_algorithm_options_to_feature(f, "lazy", n + 3);

    return f;
}

} // namespace

namespace downward::cli::search_algorithms {

void add_lazy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_lazy_search_to_namespace(n);
}

} // namespace downward::cli::search_algorithms
