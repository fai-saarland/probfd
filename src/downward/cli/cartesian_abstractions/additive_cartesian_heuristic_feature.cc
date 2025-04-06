#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cartesian_abstractions/additive_cartesian_heuristic.h"
#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"
#include "downward/cartesian_abstractions/cost_saturation.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/rng_options.h"

using namespace std;
using namespace downward;
using namespace downward::cartesian_abstractions;
using namespace downward::utils;

using namespace downward::cli;
using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class AdditiveCartesianHeuristicFeature
    : public TypedFeature<Evaluator, AdditiveCartesianHeuristic> {
public:
    AdditiveCartesianHeuristicFeature()
        : TypedFeature("cegar")
    {
        document_title("Additive CEGAR heuristic");
        document_synopsis(
            "See the paper introducing Counterexample-guided Abstraction "
            "Refinement (CEGAR) for classical planning:" +
            format_conference_reference(
                {"Jendrik Seipp", "Malte Helmert"},
                "Counterexample-guided Cartesian Abstraction Refinement",
                "https://ai.dmi.unibas.ch/papers/seipp-helmert-icaps2013.pdf",
                "Proceedings of the 23rd International Conference on Automated "
                "Planning and Scheduling (ICAPS 2013)",
                "347-351",
                "AAAI Press",
                "2013") +
            "and the paper showing how to make the abstractions additive:" +
            format_conference_reference(
                {"Jendrik Seipp", "Malte Helmert"},
                "Diverse and Additive Cartesian Abstraction Heuristics",
                "https://ai.dmi.unibas.ch/papers/seipp-helmert-icaps2014.pdf",
                "Proceedings of the 24th International Conference on "
                "Automated Planning and Scheduling (ICAPS 2014)",
                "289-297",
                "AAAI Press",
                "2014") +
            "For more details on Cartesian CEGAR and saturated cost "
            "partitioning, "
            "see the journal paper" +
            format_journal_reference(
                {"Jendrik Seipp", "Malte Helmert"},
                "Counterexample-Guided Cartesian Abstraction Refinement for "
                "Classical Planning",
                "https://ai.dmi.unibas.ch/papers/seipp-helmert-jair2018.pdf",
                "Journal of Artificial Intelligence Research",
                "62",
                "535-577",
                "2018"));

        add_list_option<shared_ptr<SubtaskGenerator>>(
            "subtasks",
            "subtask generators",
            "[landmarks(),goals()]");
        add_option<int>(
            "max_states",
            "maximum sum of abstract states over all abstractions",
            "infinity",
            Bounds("1", "infinity"));
        add_option<int>(
            "max_transitions",
            "maximum sum of real transitions (excluding self-loops) over "
            " all abstractions",
            "1M",
            Bounds("0", "infinity"));
        add_option<double>(
            "max_time",
            "maximum time in seconds for building abstractions",
            "infinity",
            Bounds("0.0", "infinity"));
        add_option<PickSplit>(
            "pick",
            "how to choose on which variable to split the flaw state",
            "max_refined");
        add_option<bool>(
            "use_general_costs",
            "allow negative costs in cost partitioning",
            "true");
        add_rng_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "cegar");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<AdditiveCartesianHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<AdditiveCartesianHeuristic>(
            opts.get_list<shared_ptr<SubtaskGenerator>>("subtasks"),
            opts.get<int>("max_states"),
            opts.get<int>("max_transitions"),
            opts.get<double>("max_time"),
            opts.get<PickSplit>("pick"),
            opts.get<bool>("use_general_costs"),
            get_rng_arguments_from_options(opts),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<AdditiveCartesianHeuristicFeature> _plugin;

TypedEnumPlugin<PickSplit> _enum_plugin(
    {{"random", "select a random variable (among all eligible variables)"},
     {"min_unwanted",
      "select an eligible variable which has the least unwanted values "
      "(number of values of v that land in the abstract state whose "
      "h-value will probably be raised) in the flaw state"},
     {"max_unwanted",
      "select an eligible variable which has the most unwanted values "
      "(number of values of v that land in the abstract state whose "
      "h-value will probably be raised) in the flaw state"},
     {"min_refined",
      "select an eligible variable which is the least refined "
      "(-1 * (remaining_values(v) / original_domain_size(v))) "
      "in the flaw state"},
     {"max_refined",
      "select an eligible variable which is the most refined "
      "(-1 * (remaining_values(v) / original_domain_size(v))) "
      "in the flaw state"},
     {"min_hadd",
      "select an eligible variable with minimal h^add(s_0) value "
      "over all facts that need to be removed from the flaw state"},
     {"max_hadd",
      "select an eligible variable with maximal h^add(s_0) value "
      "over all facts that need to be removed from the flaw state"}});

} // namespace
