#include "downward/cli/open_lists/alternation_open_list_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/open_lists/alternation_open_list.h"

using namespace std;
using namespace downward::utils;
using namespace downward::alternation_open_list;

using namespace language::plugins;

namespace {

template <typename T>
InternalFunctionDefinitionBase&
add_alternation_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &language::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            AlternationOpenListFactory<T>,
            const std::vector<shared_ptr<
                downward::TaskDependentFactory<downward::OpenList<T>>>>&,
            int>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Alternation state open list");
    } else {
        f.document_title("Alternation edge open list");
    }

    f.document_synopsis("alternates between several open lists.");

    f.make_required_argument(
        0,
        "sublists",
        "open lists between which this one alternates");
    f.make_optional_argument_with_default(
        1,
        "boost",
        "0",
        "boost value for contained open lists that are restricted "
        "to preferred successors");

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_alternation_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_alternation_open_list_to_namespace<StateOpenListEntry>(n, "state_alt");
    add_alternation_open_list_to_namespace<EdgeOpenListEntry>(n, "edge_alt");
}

} // namespace downward::cli::open_lists
