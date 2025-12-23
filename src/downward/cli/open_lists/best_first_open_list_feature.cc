#include "downward/cli/open_lists/best_first_open_list_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/open_lists/best_first_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::standard_scalar_open_list;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::add_open_list_options_to_feature;

namespace {

template <typename T>
InternalFunctionDefinitionBase&
add_best_first_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &language::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            BestFirstOpenListFactory<T>,
            bool>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Best-first state open list");
    } else {
        f.document_title("Best-first edge open list");
    }
    f.document_synopsis(
        "Open list that uses a single evaluator and FIFO tiebreaking.");

    f.document_note(
        "Implementation Notes",
        "Elements with the same evaluator value are stored in double-ended "
        "queues, called \"buckets\". The open list stores a map from "
        "evaluator "
        "values to buckets. Pushing and popping from a bucket runs in "
        "constant "
        "time. Therefore, inserting and removing an entry from the open "
        "list "
        "takes time O(log(n)), where n is the number of buckets.");

    add_open_list_options_to_feature(f, 0);

    return f;
}

template <typename T>
InternalFunctionDefinitionBase& add_best_first_open_list_with_eval_to_namespace(
    Namespace& nspace,
    std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &language::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            BestFirstOpenListFactory<T>,
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>,
            bool>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Best-first state open list");
    } else {
        f.document_title("Best-first edge open list");
    }

    f.document_synopsis(
        "Open list that uses a single evaluator and FIFO tiebreaking.");

    f.document_note(
        "Implementation Notes",
        "Elements with the same evaluator value are stored in double-ended "
        "queues, called \"buckets\". The open list stores a map from "
        "evaluator "
        "values to buckets. Pushing and popping from a bucket runs in "
        "constant "
        "time. Therefore, inserting and removing an entry from the open "
        "list "
        "takes time O(log(n)), where n is the number of buckets.");

    f.make_required_argument(0, "eval", "evaluator");
    add_open_list_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_best_first_open_list_features(Namespace& nspace)
{
    add_best_first_open_list_to_namespace<StateOpenListEntry>(
        nspace,
        "state_single");
    add_best_first_open_list_to_namespace<EdgeOpenListEntry>(
        nspace,
        "edge_single");

    add_best_first_open_list_with_eval_to_namespace<StateOpenListEntry>(
        nspace,
        "state_single_with_eval");
    add_best_first_open_list_with_eval_to_namespace<EdgeOpenListEntry>(
        nspace,
        "edge_single_with_eval");
}

} // namespace downward::cli::open_lists
