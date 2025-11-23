#include "downward/cli/search_algorithms/astar_feature.h"
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
class AStarFactory : public TaskDependentFactory<SearchAlgorithm> {
    std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory;
    std::shared_ptr<TaskDependentFactory<Evaluator>> lazy_evaluator_factory;
    std::shared_ptr<PruningMethod> pruning;
    OperatorCost cost_type;
    int bound;
    utils::FSeconds max_time;
    const std::string& description;
    utils::Verbosity verbosity;

public:
    explicit AStarFactory(
        std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory,
        std::shared_ptr<TaskDependentFactory<Evaluator>> lazy_evaluator_factory,
        std::shared_ptr<PruningMethod> pruning,
        OperatorCost cost_type,
        int bound,
        utils::FSeconds max_time,
        const std::string& description,
        utils::Verbosity verbosity)
        : eval_factory(std::move(eval_factory))
        , lazy_evaluator_factory(std::move(lazy_evaluator_factory))
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
        auto eval = eval_factory->create_object(task);

        auto [open_list, f_eval] =
            search_common::create_astar_open_list_and_f_eval(
                std::move(eval),
                verbosity);

        std::shared_ptr<Evaluator> lazy_evaluator =
            lazy_evaluator_factory ? lazy_evaluator_factory->create_object(task)
                                   : nullptr;

        return std::make_unique<EagerSearch>(
            std::move(open_list),
            true,
            std::move(f_eval),
            vector<shared_ptr<Evaluator>>{},
            std::move(lazy_evaluator),
            pruning,
            task,
            cost_type,
            bound,
            max_time,
            description,
            verbosity);
    }
};

shared_ptr<TaskDependentFactory<SearchAlgorithm>> create_astar(
    std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory,
    std::shared_ptr<TaskDependentFactory<Evaluator>> lazy_evaluator_factory,
    std::shared_ptr<PruningMethod> pruning,
    OperatorCost cost_type,
    int bound,
    utils::FSeconds max_time,
    const std::string& description,
    utils::Verbosity verbosity)
{
    return make_shared<AStarFactory>(
        std::move(eval_factory),
        std::move(lazy_evaluator_factory),
        std::move(pruning),
        cost_type,
        bound,
        max_time,
        description,
        verbosity);
}

shared_ptr<TaskDependentFactory<SearchAlgorithm>>
create_astar_no_lazy_evaluator(
    std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory,
    std::shared_ptr<PruningMethod> pruning,
    OperatorCost cost_type,
    int bound,
    utils::FSeconds max_time,
    const std::string& description,
    utils::Verbosity verbosity)
{
    return create_astar(
        std::move(eval_factory),
        nullptr,
        std::move(pruning),
        cost_type,
        bound,
        max_time,
        description,
        verbosity);
}

Feature& add_astar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin("astar", create_astar);

    f.document_title("A* search (eager)");
    f.document_synopsis(
        "A* is a special case of eager best first search that uses g+h "
        "as f-function. "
        "We break ties using the evaluator. Closed nodes are re-opened.");
    f.document_note(
        "lazy_evaluator",
        "When a state s is taken out of the open list, the lazy evaluator "
        "h "
        "re-evaluates s. If h(s) changes (for example because h is "
        "path-dependent), "
        "s is not expanded, but instead reinserted into the open list. "
        "This option is currently only present for the A* algorithm.");
    f.document_note(
        "Equivalent statements using general eager search",
        "\n```\n--search astar(evaluator)\n```\n"
        "is equivalent to\n"
        "```\n--evaluator h=evaluator\n"
        "--search eager(tiebreaking([sum([g(), h]), h], "
        "unsafe_pruning=false),\n"
        "               reopen_closed=true, f_eval=sum([g(), h]))\n"
        "```\n",
        true);

    f.make_required_argument(0, "eval", "evaluator for h-value");
    add_eager_search_options_to_feature(f, "astar", 1);

    return f;
}

Feature& add_astar_no_lazy_evaluator_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "astar_no_lazy_eval",
        create_astar_no_lazy_evaluator);

    f.document_title("A* search (eager)");
    f.document_synopsis(
        "A* is a special case of eager best first search that uses g+h "
        "as f-function. "
        "We break ties using the evaluator. Closed nodes are re-opened.");
    f.document_note(
        "Equivalent statements using general eager search",
        "\n```\n--search astar(evaluator)\n```\n"
        "is equivalent to\n"
        "```\n--evaluator h=evaluator\n"
        "--search eager(tiebreaking([sum([g(), h]), h], "
        "unsafe_pruning=false),\n"
        "               reopen_closed=true, f_eval=sum([g(), h]))\n"
        "```\n",
        true);

    f.make_required_argument(0, "eval", "evaluator for h-value");
    add_eager_search_options_to_feature(f, "astar", 1);

    return f;
}

} // namespace

namespace downward::cli::search_algorithms {

void add_astar_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_astar_to_namespace(n);
    add_astar_no_lazy_evaluator_to_namespace(n);
}

} // namespace downward::cli::search_algorithms
