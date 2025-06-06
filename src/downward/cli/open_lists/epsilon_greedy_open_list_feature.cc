#include "downward/cli/plugins/plugin.h"

#include "downward/cli/open_list_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/open_lists/epsilon_greedy_open_list.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::epsilon_greedy_open_list;

using namespace downward::cli::plugins;

using downward::cli::add_open_list_options_to_feature;
using downward::cli::get_open_list_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class EpsilonGreedyOpenListFeature
    : public TypedFeature<
          downward::OpenListFactory,
          EpsilonGreedyOpenListFactory> {
public:
    EpsilonGreedyOpenListFeature()
        : TypedFeature("epsilon_greedy")
    {
        document_title("Epsilon-greedy open list");
        document_synopsis(
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

        add_option<shared_ptr<downward::Evaluator>>("eval", "evaluator");
        add_option<double>(
            "epsilon",
            "probability for choosing the next entry randomly",
            "0.2",
            Bounds("0.0", "1.0"));
        add_rng_options_to_feature(*this);
        add_open_list_options_to_feature(*this);
    }

    virtual shared_ptr<EpsilonGreedyOpenListFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<EpsilonGreedyOpenListFactory>(
            opts.get<shared_ptr<downward::Evaluator>>("eval"),
            opts.get<double>("epsilon"),
            get_rng_arguments_from_options(opts),
            get_open_list_arguments_from_options(opts));
    }
};

FeaturePlugin<EpsilonGreedyOpenListFeature> _plugin;

} // namespace
