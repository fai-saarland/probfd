#include "downward/cli/plugins/plugin.h"

#include "downward/cli/search_algorithms/eager_search_options.h"

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

class EagerSearchFactory : public TaskDependentFactory<SearchAlgorithm> {
    std::shared_ptr<TaskDependentFactory<StateOpenList>> open_list_factory;
    bool reopen_closed;
    std::shared_ptr<TaskDependentFactory<Evaluator>> f_eval_factory;
    std::vector<std::shared_ptr<TaskDependentFactory<Evaluator>>>
        preferred_factories;
    std::shared_ptr<PruningMethod> pruning;
    OperatorCost cost_type;
    int bound;
    utils::Duration max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit EagerSearchFactory(
        std::shared_ptr<TaskDependentFactory<StateOpenList>> open_list_factory,
        bool reopen_closed,
        std::shared_ptr<TaskDependentFactory<Evaluator>> f_eval_factory,
        std::vector<std::shared_ptr<TaskDependentFactory<Evaluator>>>
            preferred_factories,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        utils::Duration max_time,
        const std::string& description,
        utils::Verbosity verbosity)
        : open_list_factory(std::move(open_list_factory))
        , reopen_closed(reopen_closed)
        , f_eval_factory(std::move(f_eval_factory))
        , preferred_factories(std::move(preferred_factories))
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
        std::vector<std::shared_ptr<Evaluator>> preferred;

        for (auto& preferred_factory : preferred_factories) {
            preferred.emplace_back(preferred_factory->create_object(task));
        }

        return std::make_unique<EagerSearch>(
            open_list_factory->create_object(task),
            reopen_closed,
            f_eval_factory ? f_eval_factory->create_object(task) : nullptr,
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

class EagerSearchFeature
    : public TypedFeature<
          TaskDependentFactory<SearchAlgorithm>,
          EagerSearchFactory> {
public:
    EagerSearchFeature()
        : TypedFeature("eager")
    {
        document_title("Eager best-first search");
        document_synopsis("");

        add_option<shared_ptr<TaskDependentFactory<StateOpenList>>>(
            "open",
            "open list");
        add_option<bool>("reopen_closed", "reopen closed nodes", "false");
        add_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "f_eval",
            "set evaluator for jump statistics. "
            "(Optional; if no evaluator is used, jump statistics will not be "
            "displayed.)",
            ArgumentInfo::NO_DEFAULT);
        add_list_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "preferred",
            "use preferred operators of these evaluators",
            "[]");
        add_eager_search_options_to_feature(*this, "eager");
    }

    shared_ptr<EagerSearchFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<EagerSearchFactory>(
            opts.get<shared_ptr<TaskDependentFactory<StateOpenList>>>("open"),
            opts.get<bool>("reopen_closed"),
            opts.get<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "f_eval",
                nullptr),
            opts.get_list<shared_ptr<TaskDependentFactory<Evaluator>>>(
                "preferred"),
            get_eager_search_arguments_from_options(opts));
    }
};

FeaturePlugin<EagerSearchFeature> _plugin;

} // namespace
