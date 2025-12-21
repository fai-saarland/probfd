#include "downward/cli/open_lists/epsilon_greedy_open_list_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/epsilon_greedy_open_list.h"

#include "downward/utils/markup.h"

#include <downward/task_dependent_factory.h>

using namespace std;
using namespace downward::utils;
using namespace downward::epsilon_greedy_open_list;

using namespace language::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

template <typename T>
InternalFunctionDefinitionBase&
add_epsilon_greedy_open_list_to_namespace(Namespace& nspace, std::string name)
{
    auto& f = nspace.insert_function_definition(
        std::move(name),
        &language::plugins::construct_shared<
            downward::TaskDependentFactory<downward::OpenList<T>>,
            EpsilonGreedyOpenListFactory<T>,
            std::shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>,
            double,
            std::shared_ptr<RandomNumberGenerator>,
            bool>);

    if constexpr (std::same_as<T, downward::StateOpenListEntry>) {
        f.document_title("Epsilon-greedy state open list");
    } else {
        f.document_title("Epsilon-greedy edge open list");
    }

    f.document_title("Epsilon-greedy state open list");
    f.document_synopsis(
        "Chooses an entry uniformly randomly with probability "
        "'epsilon', otherwise it returns the minimum entry. "
        "The algorithm is based on" +
        format_conference_reference(
            {"Richard Valenzano",
             "Nathan R. Sturtevant",
             "Jonathan Schaeffer",
             "Fan Xie"},
            "A Comparison of Knowledge-Based GBFS Enhancements and"
            " Knowledge-Free Exploration",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/"
            "7943/8066",
            "Proceedings of the Twenty-Fourth International Conference"
            " on Automated Planning and Scheduling (ICAPS 2014)",
            "375-379",
            "AAAI Press",
            "2014"));

    f.make_required_argument(0, "eval", "evaluator");
    f.make_optional_argument_with_default(
        1,
        "epsilon",
        "0.2",
        "probability for choosing the next entry randomly");
    const auto n = add_rng_options_to_feature(f, 2);
    add_open_list_options_to_feature(f, n + 2);

    return f;
}

} // namespace

namespace downward::cli::open_lists {

void add_epsilon_greedy_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_epsilon_greedy_open_list_to_namespace<StateOpenListEntry>(
        n,
        "state_epsilon_greedy");
    add_epsilon_greedy_open_list_to_namespace<EdgeOpenListEntry>(
        n,
        "edge_epsilon_greedy");
}

} // namespace downward::cli::open_lists
