#include "downward/cli/search_algorithms/lazy_greedy_feature.h"
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
constexpr auto DEFAULT_LAZY_BOOST = "1000";

class LazyGreedySearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    OperatorCost cost_type;
    int bound;
    utils::FSeconds max_time;
    const std::string& description;
    utils::Verbosity verbosity;
    bool reopen_closed;
    bool randomize_successors;
    bool preferred_successors_first;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories;
    int boost;

public:
    explicit LazyGreedySearchFactory(
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        const std::string& description,
        utils::Verbosity verbosity,
        bool reopen_closed,
        bool randomize_successors,
        bool preferred_successors_first,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories,
        int boost)
        : cost_type(cost_type)
        , bound(bound)
        , max_time(max_time)
        , description(description)
        , verbosity(verbosity)
        , reopen_closed(reopen_closed)
        , randomize_successors(randomize_successors)
        , preferred_successors_first(preferred_successors_first)
        , rng(std::move(rng))
        , eval_factories(std::move(eval_factories))
        , preferred_factories(std::move(preferred_factories))
        , boost(boost)
    {
    }

    unique_ptr<SearchAlgorithm>
    create_object(const SharedAbstractTask& task) override
    {
        std::vector<std::shared_ptr<Evaluator>> evals;

        for (auto& eval_factory : eval_factories) {
            evals.emplace_back(eval_factory->create_object(task));
        }

        std::vector<std::shared_ptr<Evaluator>> preferred;

        for (auto& preferred_factory : preferred_factories) {
            preferred.emplace_back(preferred_factory->create_object(task));
        }

        auto open = search_common::create_greedy_edge_open_list(
            evals,
            !preferred.empty(),
            boost);

        return std::make_unique<LazySearch>(
            std::move(open),
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

InternalFunctionDefinitionBase& add_lazy_greedy_search_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "lazy_greedy",
        &cli::plugins::construct_shared<
            TaskDependentFactory<SearchAlgorithm>,
            LazyGreedySearchFactory,
            OperatorCost,
            int,
            utils::FSeconds,
            const std::string&,
            utils::Verbosity,
            bool,
            bool,
            bool,
            std::shared_ptr<utils::RandomNumberGenerator>,
            vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
            vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
            int>);

    f.document_title("Greedy search (lazy)");
    f.document_synopsis("");

    f.document_note(
        "Open lists",
        "In most cases, lazy greedy best first search uses "
        "an alternation open list with one queue for each evaluator. "
        "If preferred operator evaluators are used, it adds an "
        "extra queue for each of these evaluators that includes "
        "only the nodes that are generated with a preferred operator. "
        "If only one evaluator and no preferred operator evaluator is "
        "used, "
        "the search does not use an alternation open list "
        "but a standard open list with only one queue.");
    f.document_note(
        "Equivalent statements using general lazy search",
        "\n```\n--evaluator h2=eval2\n"
        "--search lazy_greedy([eval1, h2], preferred=[h2], boost=100)"
        "\n```\n"
        "is equivalent to\n"
        "```\n--evaluator h1=eval1 --heuristic h2=eval2\n"
        "--search lazy(alt([single(h1), single(h1, pref_only=true), "
        "single(h2),\n"
        "                  single(h2, pref_only=true)], boost=100),\n"
        "              preferred=[h2])\n```\n"
        "------------------------------------------------------------\n"
        "```\n--search lazy_greedy([eval1, eval2], boost=100)\n```\n"
        "is equivalent to\n"
        "```\n--search lazy(alt([single(eval1), single(eval2)], "
        "boost=100))\n```\n"
        "------------------------------------------------------------\n"
        "```\n--evaluator h1=eval1\n--search lazy_greedy(h1, "
        "preferred=[h1])\n```\n"
        "is equivalent to\n"
        "```\n--evaluator h1=eval1\n"
        "--search lazy(alt([single(h1), single(h1, pref_only=true)], "
        "boost=1000),\n"
        "              preferred=[h1])\n```\n"
        "------------------------------------------------------------\n"
        "```\n--search lazy_greedy([eval1])\n```\n"
        "is equivalent to\n"
        "```\n--search lazy(single(eval1))\n```\n",
        true);

    f.make_required_argument(0, "evals", "evaluators");
    f.make_optional_argument_with_default(
        1,
        "boost",
        DEFAULT_LAZY_BOOST,
        "boost value for alternation queues that are restricted "
        "to preferred operator nodes");
    f.make_optional_argument_with_default(
        2,
        "reopen_closed",
        "false",
        "reopen closed nodes");
    f.make_optional_argument_with_default(
        3,
        "preferred",
        "[]",
        "use preferred operators of these evaluators");
    const auto n = add_successors_order_options_to_feature(f, 4);
    add_search_algorithm_options_to_feature(f, "lazy_greedy", n + 4);

    return f;
}

} // namespace

namespace downward::cli::search_algorithms {

void add_lazy_greedy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_lazy_greedy_search_to_namespace(n);
}

} // namespace downward::cli::search_algorithms
