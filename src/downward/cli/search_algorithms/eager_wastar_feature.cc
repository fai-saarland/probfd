#include "downward/cli/search_algorithms/eager_wastar_feature.h"
#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::eager_search;

using namespace downward::cli::eager_search;
using namespace downward::cli::plugins;

namespace {
class EagerWAstarSearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    std::vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories;
    std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>
        preferred_factories;
    bool reopen_closed;
    int boost;
    int w;
    std::shared_ptr<PruningMethod> pruning;
    OperatorCost cost_type;
    int bound;
    utils::Duration max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit EagerWAstarSearchFactory(
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>> eval_factories,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>
            preferred_factories,
        bool reopen_closed,
        int boost,
        int w,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity)
        : eval_factories(std::move(eval_factories))
        , preferred_factories(std::move(preferred_factories))
        , reopen_closed(reopen_closed)
        , boost(boost)
        , w(w)
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

        auto open = search_common::create_wastar_state_open_list(
            evals,
            !preferred.empty(),
            boost,
            w,
            verbosity);

        return std::make_unique<EagerSearch>(
            std::move(open),
            reopen_closed,
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

class EagerWAstarSearchFeature
    : public TypedFeature<TaskDependentFactory<SearchAlgorithm>> {
public:
    EagerWAstarSearchFeature()
        : TypedFeature("eager_wastar")
    {
        document_title("Eager weighted A* search");
        document_synopsis("");

        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "evals",
            "evaluators");
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_option<bool>("reopen_closed", "reopen closed nodes", "true");
        add_option<int>(
            "boost",
            "boost value for preferred operator open lists",
            "0");
        add_option<int>("w", "evaluator weight", "1");
        add_eager_search_options_to_feature(*this, "eager_wastar");

        document_note(
            "Open lists and equivalent statements using general eager search",
            "See corresponding notes for \"(Weighted) A* search (lazy)\"");
        document_note(
            "Note",
            "Eager weighted A* search uses an alternation open list "
            "while A* search uses a tie-breaking open list. Consequently, "
            "\n```\n--search eager_wastar([h()], w=1)\n```\n"
            "is **not** equivalent to\n```\n--search astar(h())\n```\n");
    }

    shared_ptr<TaskDependentFactory<SearchAlgorithm>>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<EagerWAstarSearchFactory>(
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>("evals"),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            opts.get<bool>("reopen_closed"),
            opts.get<int>("boost"),
            opts.get<int>("w"),
            get_eager_search_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::search_algorithms {

void add_eager_wastar_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<EagerWAstarSearchFeature>();
}

} // namespace downward::cli::search_algorithms
