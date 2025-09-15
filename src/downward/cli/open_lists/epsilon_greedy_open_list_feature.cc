#include "downward/cli/open_lists/epsilon_greedy_open_list_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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
using downward::cli::get_open_list_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
template <typename T>
class EpsilonGreedyOpenListFeature
    : public TypedFeature<
          downward::TaskDependentFactory<downward::OpenList<T>>,
          EpsilonGreedyOpenListFactory<T>> {
public:
    EpsilonGreedyOpenListFeature()
        requires(std::same_as<T, downward::StateOpenListEntry>)
        : EpsilonGreedyOpenListFeature::TypedFeature("state_epsilon_greedy")
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

        this->template add_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "eval",
            "evaluator");
        this->template add_option<double>(
            "epsilon",
            "probability for choosing the next entry randomly",
            "0.2",
            Bounds("0.0", "1.0"));
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    EpsilonGreedyOpenListFeature()
        requires(std::same_as<T, downward::EdgeOpenListEntry>)
        : EpsilonGreedyOpenListFeature::TypedFeature("edge_epsilon_greedy")
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

        this->template add_option<
            shared_ptr<downward::TaskDependentFactory<downward::Evaluator>>>(
            "eval",
            "evaluator");
        this->template add_option<double>(
            "epsilon",
            "probability for choosing the next entry randomly",
            "0.2",
            Bounds("0.0", "1.0"));
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<EpsilonGreedyOpenListFactory<T>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<EpsilonGreedyOpenListFactory<T>>(
            opts.get<shared_ptr<
                downward::TaskDependentFactory<downward::Evaluator>>>("eval"),
            opts.get<double>("epsilon"),
            get_rng_arguments_from_options(opts),
            get_open_list_arguments_from_options(opts));
    }
};
}

namespace downward::cli::open_lists {

void add_epsilon_greedy_open_list_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        EpsilonGreedyOpenListFeature<downward::StateOpenListEntry>>();
    raw_registry.insert_feature_plugin<
        EpsilonGreedyOpenListFeature<downward::EdgeOpenListEntry>>();
}

} // namespace
