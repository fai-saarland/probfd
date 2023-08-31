#include "probfd/heuristics/occupation_measures/higher_order_hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hroc_constraints.h"
#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"
#include "probfd/heuristics/occupation_measures/pho_constraints.h"

#include "probfd/evaluator.h"
#include "probfd/task_evaluator_factory.h"

#include "downward/utils/markup.h"

#include "downward/plugins/plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace occupation_measures {

namespace {

class OccupationMeasureHeuristicFactory : public TaskEvaluatorFactory {
    const utils::LogProxy log_;
    const lp::LPSolverType lp_solver_type_;
    const std::shared_ptr<ConstraintGenerator> constraints_;

public:
    explicit OccupationMeasureHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

OccupationMeasureHeuristicFactory::OccupationMeasureHeuristicFactory(
    const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
    , lp_solver_type_(opts.get<lp::LPSolverType>("lpsolver"))
    , constraints_(opts.get<std::shared_ptr<ConstraintGenerator>>(
          "constraint_generator"))
{
}

std::unique_ptr<FDREvaluator>
OccupationMeasureHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function)
{
    return std::make_unique<OccupationMeasureHeuristic>(
        task,
        task_cost_function,
        log_,
        lp_solver_type_,
        constraints_);
}

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

        OccupationMeasureHeuristic::add_options_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HROCGenerator>());
        return std::make_shared<OccupationMeasureHeuristicFactory>(
            options_copy);
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

        OccupationMeasureHeuristic::add_options_to_feature(*this);
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HPOMGenerator>());
        return std::make_shared<OccupationMeasureHeuristicFactory>(
            options_copy);
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

        OccupationMeasureHeuristic::add_options_to_feature(*this);

        add_option<int>("projection_size", "The size of the projections", "1");
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<HigherOrderHPOMGenerator>(options));
        return std::make_shared<OccupationMeasureHeuristicFactory>(
            options_copy);
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

        OccupationMeasureHeuristic::add_options_to_feature(*this);

        add_option<std::shared_ptr<pdbs::PatternCollectionGenerator>>(
            "patterns",
            "The pattern generator used to construct the PDB collection which "
            "is subject to post-hoc optimization.",
            "det_adapter(generator=systematic(pattern_max_size=2))");
    }

    std::shared_ptr<OccupationMeasureHeuristicFactory>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options options_copy(options);
        options_copy.set<std::shared_ptr<ConstraintGenerator>>(
            "constraint_generator",
            std::make_shared<PHOGenerator>(options));
        return std::make_shared<OccupationMeasureHeuristicFactory>(
            options_copy);
    }
};

static plugins::FeaturePlugin<HROCFactoryFeature> _plugin_hroc;
static plugins::FeaturePlugin<HPOMFactoryFeature> _plugin_hpom;
static plugins::FeaturePlugin<HOHPOMFactoryFeature> _plugin_hohpom;
static plugins::FeaturePlugin<HPHOFactoryFeature> _plugin_hpho;

} // namespace
} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd