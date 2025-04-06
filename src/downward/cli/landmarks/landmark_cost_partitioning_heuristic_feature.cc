#include "downward/cli/plugins/plugin.h"

#include "downward/cli/landmarks/landmark_heuristic_options.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "downward/landmarks/landmark_cost_partitioning_heuristic.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::landmarks;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_heuristic_options_to_feature;
using downward::cli::landmarks::get_landmark_heuristic_arguments_from_options;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

namespace {

class LandmarkCostPartitioningHeuristicFeature
    : public TypedFeature<
          downward::Evaluator,
          LandmarkCostPartitioningHeuristic> {
public:
    LandmarkCostPartitioningHeuristicFeature()
        : TypedFeature("landmark_cost_partitioning")
    {
        document_title("Landmark cost partitioning heuristic");
        document_synopsis(
            "Formerly known as the admissible landmark heuristic.\n"
            "See the papers" +
            downward::utils::format_conference_reference(
                {"Erez Karpas", "Carmel Domshlak"},
                "Cost-Optimal Planning with Landmarks",
                "https://www.ijcai.org/Proceedings/09/Papers/288.pdf",
                "Proceedings of the 21st International Joint Conference on "
                "Artificial Intelligence (IJCAI 2009)",
                "1728-1733",
                "AAAI Press",
                "2009") +
            "and" +
            downward::utils::format_conference_reference(
                {"Emil Keyder and Silvia Richter and Malte Helmert"},
                "Sound and Complete Landmarks for And/Or Graphs",
                "https://ai.dmi.unibas.ch/papers/keyder-et-al-ecai2010.pdf",
                "Proceedings of the 19th European Conference on Artificial "
                "Intelligence (ECAI 2010)",
                "335-340",
                "IOS Press",
                "2010"));

        /*
          We usually have the options of base classes behind the options
          of specific implementations. In the case of landmark
          heuristics, we decided to have the common options at the front
          because it feels more natural to specify the landmark factory
          before the more specific arguments like the used LP solver in
          the case of an optimal cost partitioning heuristic.
        */
        add_landmark_heuristic_options_to_feature(
            *this,
            "landmark_cost_partitioning");
        add_option<CostPartitioningMethod>(
            "cost_partitioning",
            "strategy for partitioning operator costs among landmarks",
            "uniform");
        add_option<bool>("alm", "use action landmarks", "true");
        add_lp_solver_option_to_feature(*this);

        document_note(
            "Usage with A*",
            "We recommend to add this heuristic as lazy_evaluator when using "
            "it in the A* algorithm. This way, the heuristic is recomputed "
            "before a state is expanded, leading to improved estimates that "
            "incorporate all knowledge gained from paths that were found after "
            "the state was inserted into the open list.");
        document_note(
            "Consistency",
            "The heuristic is consistent along single paths if it is "
            "set as lazy_evaluator; i.e. when expanding s then we have "
            "h(s) <= h(s')+cost(a) for all successors s' of s reached "
            "with a. But newly found paths to s can increase h(s), at "
            "which point the above inequality might not hold anymore.");
        document_note(
            "Optimal Cost Partitioning",
            "To use ``cost_partitioning=optimal``, you must build the planner "
            "with LP support. See [build instructions "
            "https://github.com/aibasel/downward/blob/main/BUILD.md].");
        document_note(
            "Preferred operators",
            "Preferred operators should not be used for optimal planning. "
            "See Heuristic#Landmark_sum_heuristic for more information "
            "on using preferred operators; the comments there also apply "
            "to this heuristic.");

        document_language_support("action costs", "supported");
        document_language_support(
            "conditional_effects",
            "supported if the LandmarkFactory supports them; otherwise "
            "not supported");
        document_language_support("axioms", "not allowed");

        document_property("admissible", "yes");
        document_property(
            "consistent",
            "no; see document note about consistency");
        document_property("safe", "yes");
    }

    virtual shared_ptr<LandmarkCostPartitioningHeuristic>
    create_component(const Options& opts, const downward::utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<LandmarkCostPartitioningHeuristic>(
            get_landmark_heuristic_arguments_from_options(opts),
            opts.get<CostPartitioningMethod>("cost_partitioning"),
            opts.get<bool>("alm"),
            get_lp_solver_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkCostPartitioningHeuristicFeature> _plugin;

TypedEnumPlugin<CostPartitioningMethod> _enum_plugin({
    {"optimal", "use optimal (LP-based) cost partitioning"},
    {"uniform",
     "partition operator costs uniformly among all landmarks "
     "achieved by that operator"},
});

} // namespace
