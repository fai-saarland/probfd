#include "downward/cli/search_algorithms/eager_feature.h"
#include "downward/cli/search_algorithms/eager_search_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
    utils::FSeconds max_time;
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
        utils::FSeconds max_time,
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

shared_ptr<TaskDependentFactory<SearchAlgorithm>> create_eager_search(
    std::shared_ptr<TaskDependentFactory<StateOpenList>> open_list_factory,
    bool reopen_closed,
    std::shared_ptr<TaskDependentFactory<Evaluator>> f_eval_factory,
    std::vector<std::shared_ptr<TaskDependentFactory<Evaluator>>>
        preferred_factories,
    std::shared_ptr<PruningMethod> pruning,
    OperatorCost cost_type,
    int bound,
    utils::FSeconds max_time,
    const std::string& description,
    utils::Verbosity verbosity)
{
    return make_shared_from_arg_tuples<EagerSearchFactory>(
        std::move(open_list_factory),
        reopen_closed,
        std::move(f_eval_factory),
        std::move(preferred_factories),
        std::move(pruning),
        cost_type,
        bound,
        max_time,
        description,
        verbosity);
}

shared_ptr<TaskDependentFactory<SearchAlgorithm>>
create_eager_search_no_f_evaluator(
    std::shared_ptr<TaskDependentFactory<StateOpenList>> open_list_factory,
    bool reopen_closed,
    std::vector<std::shared_ptr<TaskDependentFactory<Evaluator>>>
        preferred_factories,
    std::shared_ptr<PruningMethod> pruning,
    OperatorCost cost_type,
    int bound,
    utils::FSeconds max_time,
    const std::string& description,
    utils::Verbosity verbosity)
{
    return create_eager_search(
        std::move(open_list_factory),
        reopen_closed,
        nullptr,
        std::move(preferred_factories),
        std::move(pruning),
        cost_type,
        bound,
        max_time,
        description,
        verbosity);
}

InternalFunctionDefinitionBase& add_eager_search_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition("eager", create_eager_search);

    f.document_title("Eager best-first search");
    f.document_synopsis("");

    f.make_required_argument(0, "open", "open list");
    f.make_optional_argument_with_default(
        1,
        "reopen_closed",
        "false",
        "reopen closed nodes");
    f.make_required_argument(2, "f_eval", "set evaluator for jump statistics.");
    f.make_optional_argument_with_default(
        3,
        "preferred",
        "[]",
        "use preferred operators of these evaluators");
    add_eager_search_options_to_feature(f, "eager", 4);

    return f;
}

InternalFunctionDefinitionBase& add_eager_search_no_f_evaluator_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "eager_no_f_evaluator",
        create_eager_search_no_f_evaluator);

    f.document_title("Eager best-first search");
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
    add_eager_search_options_to_feature(f, "eager", 3);

    return f;
}

} // namespace

namespace downward::cli::search_algorithms {

void add_eager_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_eager_search_to_namespace(n);
    add_eager_search_no_f_evaluator_to_namespace(n);
}

} // namespace downward::cli::search_algorithms
