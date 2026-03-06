#include "downward/cli/search_algorithms/eager_wastar_feature.h"
#include "downward/cli/search_algorithms/eager_search_options.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/search_algorithms/eager_search.h"
#include "downward/search_algorithms/search_common.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::eager_search;

using namespace downward::cli::eager_search;
using namespace language::parser;

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
    utils::FSeconds max_time;
    std::string description;
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
        utils::FSeconds max_time,
        std::string description,
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
        , description(std::move(description))
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

} // namespace

namespace downward::cli::search_algorithms {

InternalFunctionDefinitionBase&
add_eager_wastar_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<SearchAlgorithm>,
        EagerWAstarSearchFactory,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
        std::vector<shared_ptr<TaskDependentFactory<Evaluator>>>,
        bool,
        int,
        int,
        std::shared_ptr<PruningMethod>,
        OperatorCost,
        int,
        utils::FSeconds,
        std::string,
        utils::Verbosity>>(nspace, "eager_wastar");

    f.document_title("Eager weighted A* search");
    f.document_synopsis("");
    f.document_note(
        "Open lists and equivalent statements using general eager search",
        "See corresponding notes for \"(Weighted) A* search (lazy)\"");
    f.document_note(
        "Note",
        "Eager weighted A* search uses an alternation open list "
        "while A* search uses a tie-breaking open list. Consequently, "
        "\n```\n--search eager_wastar([h()], w=1)\n```\n"
        "is **not** equivalent to\n```\n--search astar(h())\n```\n");

    f.make_required_argument(0, "evals", "evaluators");
    f.make_optional_argument_with_default(
        1,
        "preferred",
        "[]",
        "use preferred operators of these evaluators");
    f.make_optional_argument_with_default(
        2,
        "reopen_closed",
        "true",
        "reopen closed nodes");
    f.make_optional_argument_with_default(
        3,
        "boost",
        "0",
        "boost value for preferred operator open lists");
    f.make_optional_argument_with_default(4, "w", "1", "evaluator weight");
    add_eager_search_options_to_feature(f, "eager_wastar", 5);

    return f;
}

} // namespace downward::cli::search_algorithms
