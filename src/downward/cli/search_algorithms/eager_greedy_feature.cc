#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;

using namespace downward::cli::eager_search;
using namespace downward::cli::plugins;

namespace {

class EagerGreedySearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    std::vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories;
    std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>
        preferred_factories;
    int boost;
    std::shared_ptr<PruningMethod> pruning;
    OperatorCost cost_type;
    int bound;
    utils::Duration max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit EagerGreedySearchFactory(
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>
            preferred_factories,
        int boost,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity)
        : eval_factories(std::move(eval_factories))
        , preferred_factories(std::move(preferred_factories))
        , boost(boost)
        , pruning(pruning)
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
        std::vector<std::shared_ptr<Evaluator>> evals;

        for (auto& eval_factory : eval_factories) {
            evals.emplace_back(eval_factory->create_object(task));
        }

        std::vector<std::shared_ptr<Evaluator>> preferred;

        for (auto& preferred_factory : preferred_factories) {
            preferred.emplace_back(preferred_factory->create_object(task));
        }

        auto open = search_common::create_greedy_state_open_list(
            evals,
            !preferred.empty(),
            boost);

        return std::make_unique<eager_search::EagerSearch>(
            std::move(open),
            false,
            nullptr,
            std::move(preferred),
            nullptr,
            pruning,
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

class EagerGreedySearchFeature
    : public TypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          EagerGreedySearchFactory> {
public:
    EagerGreedySearchFeature()
        : TypedFeature("eager_greedy")
    {
        document_title("Greedy search (eager)");
        document_synopsis("");

        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "evals",
            "evaluators");
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_option<int>(
            "boost",
            "boost value for preferred operator open lists",
            "0");
        add_eager_search_options_to_feature(*this, "eager_greedy");

        document_note(
            "Open list",
            "In most cases, eager greedy best first search uses "
            "an alternation open list with one queue for each evaluator. "
            "If preferred operator evaluators are used, it adds an extra queue "
            "for each of these evaluators that includes only the nodes that "
            "are generated with a preferred operator. "
            "If only one evaluator and no preferred operator evaluator is "
            "used, "
            "the search does not use an alternation open list but a "
            "standard open list with only one queue.");
        document_note("Closed nodes", "Closed node are not re-opened");
        document_note(
            "Equivalent statements using general eager search",
            "\n```\n--evaluator h2=eval2\n"
            "--search eager_greedy([eval1, h2], preferred=[h2], boost=100)"
            "\n```\n"
            "is equivalent to\n"
            "```\n--evaluator h1=eval1 --heuristic h2=eval2\n"
            "--search eager(alt([single(h1), single(h1, pref_only=true), "
            "single(h2), \n"
            "                    single(h2, pref_only=true)], boost=100),\n"
            "               preferred=[h2])\n```\n"
            "------------------------------------------------------------\n"
            "```\n--search eager_greedy([eval1, eval2])\n```\n"
            "is equivalent to\n"
            "```\n--search eager(alt([single(eval1), single(eval2)]))\n```\n"
            "------------------------------------------------------------\n"
            "```\n--evaluator h1=eval1\n"
            "--search eager_greedy(h1, preferred=[h1])\n```\n"
            "is equivalent to\n"
            "```\n--evaluator h1=eval1\n"
            "--search eager(alt([single(h1), single(h1, pref_only=true)]),\n"
            "               preferred=[h1])\n```\n"
            "------------------------------------------------------------\n"
            "```\n--search eager_greedy([eval1])\n```\n"
            "is equivalent to\n"
            "```\n--search eager(single(eval1))\n```\n",
            true);
    }

    shared_ptr<EagerGreedySearchFactory>
    create_component(const Options& opts, const utils::Context& context)
        const override
    {
        verify_list_non_empty<shared_ptr<Evaluator>>(context, opts, "evals");

        return make_shared_from_arg_tuples<EagerGreedySearchFactory>(
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>("evals"),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            opts.get<int>("boost"),
            get_eager_search_arguments_from_options(opts));
    }
};

FeaturePlugin<EagerGreedySearchFeature> _plugin;

} // namespace
