#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/heuristics/occupation_measures/higher_order_hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hroc_constraints.h"
#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"
#include "probfd/heuristics/occupation_measures/pho_constraints.h"

#include "utils/markup.h"

#include "plugins/plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace occupation_measures {

class HROCFeature
    : public plugins::TypedFeature<TaskEvaluator, OccupationMeasureHeuristic> {
public:
    HROCFeature()
        : TypedFeature("hroc")
    {
        document_title("Regrouped operator-counting heuristic");
        document_synopsis(
            "For details, see" +
            utils::format_conference_reference(
                {"Felipe Trevizan", "Sylvie Thiebaux", "Patrik Haslum"},
                "Occupation Measure Heuristics for Probabilistic Planning",
                "https://ojs.aaai.org/index.php/ICAPS/article/view/13840",
                "Proceedings of 27th International Conference on Automated"
                "Planning and Scheduling (ICAPS 2017)",
                "306-315",
                "AAAI Press",
                "2017"));

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        OccupationMeasureHeuristic::add_options_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristic>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HROCGenerator>());
        return std::make_shared<OccupationMeasureHeuristic>(options_copy);
    }
};

class HPOMFeature
    : public plugins::TypedFeature<TaskEvaluator, OccupationMeasureHeuristic> {
public:
    HPOMFeature()
        : TypedFeature("hpom")
    {
        document_title("Projection Occupation Measure Heuristic");

        document_synopsis(
            "The projection occupation measure heuristic. "
            "For details, see" +
            utils::format_conference_reference(
                {"Felipe Trevizan", "Sylvie Thiebaux", "Patrik Haslum"},
                "Occupation Measure Heuristics for Probabilistic Planning",
                "https://ojs.aaai.org/index.php/ICAPS/article/view/13840",
                "Proceedings of 27th International Conference on Automated"
                "Planning and Scheduling (ICAPS 2017)",
                "306-315",
                "AAAI Press",
                "2017"));

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        OccupationMeasureHeuristic::add_options_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristic>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HPOMGenerator>());
        return std::make_shared<OccupationMeasureHeuristic>(options_copy);
    }
};

class HOHPOMFeature
    : public plugins::TypedFeature<TaskEvaluator, OccupationMeasureHeuristic> {
public:
    HOHPOMFeature()
        : TypedFeature("ho_hpom")
    {
        document_title("Higher-Order Projection Occupation Measure Heuristic");
        document_synopsis(
            "The projection occupation measure heuristic with general "
            "projections"
            "beyond atomic projections. "
            "For details, see" +
            utils::format_conference_reference(
                {"Felipe Trevizan", "Sylvie Thiebaux", "Patrik Haslum"},
                "Occupation Measure Heuristics for Probabilistic Planning",
                "https://ojs.aaai.org/index.php/ICAPS/article/view/13840",
                "Proceedings of 27th International Conference on Automated"
                "Planning and Scheduling (ICAPS 2017)",
                "306-315",
                "AAAI Press",
                "2017"));

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        OccupationMeasureHeuristic::add_options_to_feature(*this);

        add_option<int>("projection_size", "The size of the projections", "1");
    }

    std::shared_ptr<OccupationMeasureHeuristic>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HigherOrderHPOMGenerator>(options));
        return std::make_shared<OccupationMeasureHeuristic>(options_copy);
    }
};

class HPHOFeature
    : public plugins::TypedFeature<TaskEvaluator, OccupationMeasureHeuristic> {
public:
    HPHOFeature()
        : TypedFeature("pho")
    {
        document_title("Post-hoc Optimization Heuristic");

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        OccupationMeasureHeuristic::add_options_to_feature(*this);

        add_option<std::shared_ptr<pdbs::PatternCollectionGenerator>>(
            "patterns",
            "The pattern generator used to construct the PDB collection which "
            "is subject to post-hoc optimization.",
            "det_adapter(generator=systematic(pattern_max_size=2))");
    }

    std::shared_ptr<OccupationMeasureHeuristic>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<PHOGenerator>(options));
        return std::make_shared<OccupationMeasureHeuristic>(options_copy);
    }
};

static plugins::FeaturePlugin<HROCFeature> _plugin_hroc;
static plugins::FeaturePlugin<HPOMFeature> _plugin_hpom;
static plugins::FeaturePlugin<HOHPOMFeature> _plugin_hohpom;
static plugins::FeaturePlugin<HPHOFeature> _plugin_hpho;

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd