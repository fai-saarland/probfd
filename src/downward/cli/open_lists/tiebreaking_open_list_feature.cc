#include "downward/cli/open_lists/tiebreaking_open_list_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/open_lists/tiebreaking_open_list.h"

using namespace std;
using namespace downward::utils;
using namespace downward::tiebreaking_open_list;

using namespace language::plugins;

using downward::cli::add_open_list_options_to_feature;

namespace {

template <typename T>
InternalFunctionDefinitionBase&
add_tiebreaking_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &language::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            TieBreakingOpenListFactory<T>,
            std::vector<std::shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>,
            bool,
            bool>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Tie-breaking state open list");
    } else {
        f.document_title("Tie-breaking edge open list");
    }

    f.document_synopsis("");

    f.make_required_argument(0, "evals", "evaluators");
    f.make_optional_argument_with_default(
        1,
        "unsafe_pruning",
        "true",
        "allow unsafe pruning when the main evaluator regards a state a "
        "dead end");
    add_open_list_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_tiebreaking_open_list_features(Namespace& nspace)
{
    add_tiebreaking_open_list_to_namespace<StateOpenListEntry>(
        nspace,
        "state_tiebreaking");
    add_tiebreaking_open_list_to_namespace<EdgeOpenListEntry>(
        nspace,
        "edge_tiebreaking");
}

} // namespace downward::cli::open_lists
