#include "downward/cli/search_algorithms/eager_greedy_feature.h"
#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
    utils::FSeconds max_time;
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
        utils::FSeconds max_time,
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
    : public SharedTypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
          std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
          int,
          std::shared_ptr<PruningMethod>,
          OperatorCost,
          int,
          utils::FSeconds,
          const std::string&,
          utils::Verbosity> {
public:
    EagerGreedySearchFeature()
        : TypedFeature("eager_greedy", &EagerGreedySearchFeature::func)
    {
        document_title("Greedy search (eager)");
        document_synopsis("");
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

        make_required_argument(0, "evals", "evaluators");
        make_optional_argument_with_default(
            1,
            "preferred",
            "[]",
            "use preferred operators of these evaluators");
        make_optional_argument_with_default(
            2,
            "boost",
            "0",
            "boost value for preferred operator open lists");
        add_eager_search_options_to_feature(*this, "eager_greedy", 3);
    }

    static shared_ptr<TaskDependentFactory<SearchAlgorithm>> func(
        const utils::Context& context,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>
            preferred_factories,
        int boost,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        const std::string& description,
        utils::Verbosity verbosity)
    {
        if (eval_factories.empty()) {
            context.error("List of evaluators may not be empty.");
        }

        return make_shared_from_arg_tuples<EagerGreedySearchFactory>(
            std::move(eval_factories),
            std::move(preferred_factories),
            boost,
            std::move(pruning),
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_eager_greedy_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<EagerGreedySearchFeature>();
}

} // namespace downward::cli::search_algorithms
