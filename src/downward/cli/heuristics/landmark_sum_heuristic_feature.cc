#include "downward/cli/heuristics/landmark_sum_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/landmarks/landmark_heuristic_options.h"

#include "downward/landmarks/landmark_sum_heuristic.h"

#include "downward/utils/markup.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::landmarks;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::landmarks::add_landmark_heuristic_options_to_feature;
using downward::cli::landmarks::get_landmark_heuristic_arguments_from_options;

namespace {
class LandmarkSumHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    shared_ptr<LandmarkFactory> landmark_factory;
    bool pref;
    bool prog_goal;
    bool prog_gn;
    bool prog_r;

public:
    LandmarkSumHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        shared_ptr<LandmarkFactory> landmark_factory,
        bool pref,
        bool prog_goal,
        bool prog_gn,
        bool prog_r)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , landmark_factory(std::move(landmark_factory))
        , pref(pref)
        , prog_goal(prog_goal)
        , prog_gn(prog_gn)
        , prog_r(prog_r)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<LandmarkSumHeuristic>(
            landmark_factory,
            pref,
            prog_goal,
            prog_gn,
            prog_r,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class LandmarkSumHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          LandmarkSumHeuristicFactory> {
public:
    LandmarkSumHeuristicFeature()
        : TypedFeature("landmark_sum")
    {
        document_title("Landmark sum heuristic");
        document_synopsis(
            "Formerly known as the landmark heuristic or landmark count "
            "heuristic.\n"
            "See the papers" +
            format_conference_reference(
                {"Silvia Richter", "Malte Helmert", "Matthias Westphal"},
                "Landmarks Revisited",
                "https://ai.dmi.unibas.ch/papers/richter-et-al-aaai2008.pdf",
                "Proceedings of the 23rd AAAI Conference on Artificial "
                "Intelligence (AAAI 2008)",
                "975-982",
                "AAAI Press",
                "2008") +
            "and" +
            format_journal_reference(
                {"Silvia Richter", "Matthias Westphal"},
                "The LAMA Planner: Guiding Cost-Based Anytime Planning with "
                "Landmarks",
                "https://www.aaai.org/Papers/JAIR/Vol39/JAIR-3903.pdf",
                "Journal of Artificial Intelligence Research",
                "39",
                "127-177",
                "2010"));

        add_landmark_heuristic_options_to_feature(
            *this,
            "landmark_sum_heuristic");

        document_note(
            "Note on performance for satisficing planning",
            "The cost of a landmark is based on the cost of the "
            "operators that achieve it. For satisficing search this "
            "can be counterproductive since it is often better to "
            "focus on distance from goal (i.e. length of the plan) "
            "rather than cost. In experiments we achieved the best "
            "performance using the option 'transform=adapt_costs(one)' "
            "to enforce unit costs.");
        document_note(
            "Preferred operators",
            "Computing preferred operators is *only enabled* when setting "
            "pref=true because it has a nontrivial runtime cost. Using the "
            "heuristic for preferred operators without setting pref=true "
            "has no effect.\n"
            "Our implementation to compute preferred operators based on "
            "landmarks "
            "differs from the description in the literature (see reference "
            "above)."
            "The original implementation computes two kinds of preferred "
            "operators:\n\n"
            "+ If there is an applicable operator that reaches a landmark, all "
            "such operators are preferred.\n"
            "+ If no such operators exist, perform an FF-style relaxed "
            "exploration towards the nearest landmarks (according to the "
            "landmark orderings) and use the preferred operators of this "
            "exploration.\n\n\n"
            "Our implementation only considers preferred operators of the "
            "first "
            "type and does not include the second type. The rationale for this "
            "change is that it reduces code complexity and helps more cleanly "
            "separate landmark-based and FF-based computations in LAMA-like "
            "planner configurations. In our experiments, only considering "
            "preferred operators of the first type reduces performance when "
            "using "
            "the heuristic and its preferred operators in isolation but "
            "improves "
            "performance when using this heuristic in conjunction with the "
            "FF heuristic, as in LAMA-like planner configurations.");

        document_language_support("action costs", "supported");
        document_language_support(
            "conditional_effects",
            "supported if the LandmarkFactory supports them; otherwise "
            "ignored");
        document_language_support("axioms", "ignored");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property(
            "safe",
            "yes except on tasks with axioms or on tasks with "
            "conditional effects when using a LandmarkFactory "
            "not supporting them");
    }

    shared_ptr<LandmarkSumHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<LandmarkSumHeuristicFactory>(
            get_landmark_heuristic_arguments_from_options(opts));
    }
};
}

namespace downward::cli::heuristics {

void add_landmark_sum_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<LandmarkSumHeuristicFeature>();
}

} // namespace
