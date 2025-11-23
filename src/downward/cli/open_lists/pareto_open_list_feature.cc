#include "downward/cli/open_lists/pareto_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/pareto_open_list.h"

#include "downward/utils/memory.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pareto_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

template <typename T>
InternalFunctionDefinitionBase& add_pareto_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &downward::cli::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            ParetoOpenListFactory<T>,
            const std::vector<std::shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>&,
            bool,
            std::shared_ptr<RandomNumberGenerator>,
            bool>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Pareto state open list");
    } else {
        f.document_title("Pareto edge open list");
    }

    f.document_synopsis(
        "Selects one of the Pareto-optimal (regarding the sub-evaluators) "
        "entries for removal.");

    f.make_required_argument(0, "evals", "evaluators");
    f.make_optional_argument_with_default(
        1,
        "state_uniform_selection",
        "false",
        "When removing an entry, we select a non-dominated bucket "
        "and return its oldest entry. If this option is false, we select "
        "uniformly from the non-dominated buckets; if the option is true, "
        "we weight the buckets with the number of entries.");
    const auto n = add_rng_options_to_feature(f, 2);
    add_open_list_options_to_feature(f, n + 2);

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_pareto_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pareto_open_list_to_namespace<StateOpenListEntry>(n, "state_pareto");
    add_pareto_open_list_to_namespace<EdgeOpenListEntry>(n, "edge_pareto");
}

} // namespace downward::cli::open_lists
