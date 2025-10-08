#include "downward/cli/search_algorithms/lazy_greedy_feature.h"
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
constexpr auto DEFAULT_LAZY_BOOST = "1000";

class LazyGreedySearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    OperatorCost cost_type;
    int bound;
    utils::Duration max_time;
    const std::string& description;
    utils::Verbosity verbosity;
    bool reopen_closed;
    bool randomize_successors;
    bool preferred_successors_first;
    int random_seed;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> preferred_factories;
    int boost;

public:
    explicit LazyGreedySearchFactory(
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity,
        bool reopen_closed,
        bool randomize_successors,
        bool preferred_successors_first,
        int random_seed,
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
        , random_seed(random_seed)
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
            random_seed,
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

class LazyGreedySearchFeature
    : public SharedTypedFeature<TaskDependentFactory<SearchAlgorithm>> {
public:
    LazyGreedySearchFeature()
        : SharedTypedFeature("lazy_greedy")
    {
        document_title("Greedy search (lazy)");
        document_synopsis("");

        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "evals",
            "evaluators");
        add_option<int>(
            "boost",
            "boost value for alternation queues that are restricted "
            "to preferred operator nodes",
            DEFAULT_LAZY_BOOST);
        add_option<bool>("reopen_closed", "reopen closed nodes", "false");
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_successors_order_options_to_feature(*this);
        add_search_algorithm_options_to_feature(*this, "lazy_greedy");

        document_note(
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
        document_note(
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
    }

    shared_ptr<TaskDependentFactory<SearchAlgorithm>>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<LazyGreedySearchFactory>(
            get_search_algorithm_arguments_from_options(opts),
            opts.get<bool>("reopen_closed"),
            get_successors_order_arguments_from_options(opts),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>("evals"),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            opts.get<int>("boost"));
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_lazy_greedy_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LazyGreedySearchFeature>();
}

} // namespace downward::cli::search_algorithms
