#include "probfd/occupation_measures/higher_order_hpom_constraints.h"
#include "probfd/occupation_measures/hpom_constraints.h"
#include "probfd/occupation_measures/hroc_constraints.h"
#include "probfd/occupation_measures/pho_constraints.h"

#include "probfd/heuristics/occupation_measure_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"

#include "probfd/task_evaluator_factory.h"

#include "downward/utils/markup.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::pdbs;
using namespace probfd::occupation_measures;

namespace {

class HROCFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          OccupationMeasureHeuristicFactory> {
public:
    HROCFactoryFeature()
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

        utils::add_log_options_to_feature(*this);
        lp::add_lp_solver_option_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            OccupationMeasureHeuristicFactory>(
            utils::get_log_arguments_from_options(options),
            lp::get_lp_solver_arguments_from_options(options),
            std::make_shared<HROCGenerator>());
    }
};

class HPOMFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          OccupationMeasureHeuristicFactory> {
public:
    HPOMFactoryFeature()
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

        utils::add_log_options_to_feature(*this);
        lp::add_lp_solver_option_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            OccupationMeasureHeuristicFactory>(
            utils::get_log_arguments_from_options(options),
            lp::get_lp_solver_arguments_from_options(options),
            std::make_shared<HPOMGenerator>());
    }
};

class HOHPOMFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          OccupationMeasureHeuristicFactory> {
public:
    HOHPOMFactoryFeature()
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

        utils::add_log_options_to_feature(*this);
        lp::add_lp_solver_option_to_feature(*this);

        add_option<int>("projection_size", "The size of the projections", "1");
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            OccupationMeasureHeuristicFactory>(
            utils::get_log_arguments_from_options(options),
            lp::get_lp_solver_arguments_from_options(options),
            std::make_shared<HigherOrderHPOMGenerator>(
                options.get<int>("projection_size")));
    }
};

class HPHOFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          OccupationMeasureHeuristicFactory> {
public:
    HPHOFactoryFeature()
        : TypedFeature("pho")
    {
        document_title("Post-hoc Optimization Heuristic");

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        utils::add_log_options_to_feature(*this);
        lp::add_lp_solver_option_to_feature(*this);

        add_option<std::shared_ptr<probfd::pdbs::PatternCollectionGenerator>>(
            "patterns",
            "The pattern generator used to construct the PDB collection which "
            "is subject to post-hoc optimization.",
            "det_adapter(generator=systematic(pattern_max_size=2))");
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            OccupationMeasureHeuristicFactory>(
            utils::get_log_arguments_from_options(options),
            lp::get_lp_solver_arguments_from_options(options),
            std::make_shared<PHOGenerator>(
                options.get<std::shared_ptr<PatternCollectionGenerator>>(
                    "patterns")));
    }
};

plugins::FeaturePlugin<HROCFactoryFeature> _plugin_hroc;
plugins::FeaturePlugin<HPOMFactoryFeature> _plugin_hpom;
plugins::FeaturePlugin<HOHPOMFactoryFeature> _plugin_hohpom;
plugins::FeaturePlugin<HPHOFactoryFeature> _plugin_hpho;

} // namespace