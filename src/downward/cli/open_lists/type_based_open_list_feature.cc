#include "downward/cli/open_lists/type_based_open_list_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/type_based_open_list.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::type_based_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

template <typename T>
InternalFunctionDefinitionBase&
add_tiebreaking_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &downward::cli::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            TypeBasedOpenListFactory<T>,
            const std::vector<std::shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>&,
            std::shared_ptr<RandomNumberGenerator>>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Type-based state open list");
    } else {
        f.document_title("Type-based edge open list");
    }

    f.document_synopsis(
        "Uses multiple evaluators to assign entries to buckets. "
        "All entries in a bucket have the same evaluator values. "
        "When retrieving an entry, a bucket is chosen uniformly at "
        "random and one of the contained entries is selected "
        "uniformly randomly. "
        "The algorithm is based on" +
        format_conference_reference(
            {"Fan Xie", "Martin Mueller", "Robert Holte", "Tatsuya Imai"},
            "Type-Based Exploration with Multiple Search Queues for"
            " Satisficing Planning",
            "http://www.aaai.org/ocs/index.php/AAAI/AAAI14/paper/view/8472/"
            "8705",
            "Proceedings of the Twenty-Eigth AAAI Conference Conference"
            " on Artificial Intelligence (AAAI 2014)",
            "2395-2401",
            "AAAI Press",
            "2014"));

    f.make_required_argument(
        0,
        "evaluators",
        "Evaluators used to determine the bucket for each entry.");
    add_rng_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_type_based_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_tiebreaking_open_list_to_namespace<StateOpenListEntry>(
        n,
        "state_type_based");
    add_tiebreaking_open_list_to_namespace<EdgeOpenListEntry>(
        n,
        "edge_type_based");
}

} // namespace downward::cli::open_lists
