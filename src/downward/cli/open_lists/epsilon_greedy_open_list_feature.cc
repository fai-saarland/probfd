#include "downward/cli/open_lists/epsilon_greedy_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/open_lists/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/epsilon_greedy_open_list.h"

#include "downward/utils/markup.h"

#include <downward/task_dependent_factory.h>

using namespace std;
using namespace downward::utils;
using namespace downward::epsilon_greedy_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;

using downward::cli::utils::add_rng_options_to_feature;

namespace {
template <typename T>
class EpsilonGreedyOpenListFeature
    : public SharedTypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          const std::shared_ptr<
              downward::TaskDependentFactory<downward::Evaluator>>&,
          double,
          int,
          bool> {
public:
    EpsilonGreedyOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : EpsilonGreedyOpenListFeature::TypedFeature(
              "state_epsilon_greedy",
              &EpsilonGreedyOpenListFeature::func)
    {
        this->document_title("Epsilon-greedy state open list");
        this->document_synopsis(
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

        this->make_required_argument(0, "eval", "evaluator");
        this->make_optional_argument_with_default(
            1,
            "epsilon",
            "0.2",
            "probability for choosing the next entry randomly");
        const auto n = add_rng_options_to_feature(*this, 2);
        add_open_list_options_to_feature(*this, n + 2);
    }

    EpsilonGreedyOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : EpsilonGreedyOpenListFeature::TypedFeature(
              "edge_epsilon_greedy",
              &EpsilonGreedyOpenListFeature::func)
    {
        this->document_title("Epsilon-greedy edge open list");
        this->document_synopsis(
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

        this->make_required_argument(0, "eval", "evaluator");
        this->make_optional_argument_with_default(
            1,
            "epsilon",
            "0.2",
            "probability for choosing the next entry randomly");
        const auto n = add_rng_options_to_feature(*this, 2);
        add_open_list_options_to_feature(*this, n + 2);
    }

    static shared_ptr<downward::TaskDependentFactory<downward::OpenList<T>>>
    func(
        const Context&,
        const std::shared_ptr<
            downward::TaskDependentFactory<downward::Evaluator>>& eval_factory,
        double epsilon,
        int random_seed,
        bool pref_only)
    {
        return make_shared_from_arg_tuples<EpsilonGreedyOpenListFactory<T>>(
            eval_factory,
            epsilon,
            random_seed,
            pref_only);
    }
};
} // namespace

namespace downward::cli::open_lists {

void add_epsilon_greedy_open_list_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<
        EpsilonGreedyOpenListFeature<downward::StateOpenListEntry>>();
    n.insert_feature_plugin<
        EpsilonGreedyOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace downward::cli::open_lists
