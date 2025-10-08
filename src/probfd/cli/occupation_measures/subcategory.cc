#include "probfd/cli/occupation_measures/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/logging_options.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/occupation_measures/higher_order_hpom_constraints.h"
#include "probfd/occupation_measures/hpom_constraints.h"
#include "probfd/occupation_measures/hroc_constraints.h"
#include "probfd/occupation_measures/pho_constraints.h"

#include "probfd/heuristics/occupation_measure_heuristic.h"

#include "probfd/pdbs/pattern_collection_generator.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/markup.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::pdbs;
using namespace probfd::occupation_measures;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

namespace {
class HROCFactoryFeature : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    HROCFactoryFeature()
        : SharedTypedFeature("hroc")
    {
        document_title("Regrouped operator-counting heuristic");
        document_synopsis(
            "For details, see" +
            format_conference_reference(
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

        add_log_options_to_feature(*this);
        add_lp_solver_option_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            get_log_arguments_from_options(options),
            get_lp_solver_arguments_from_options(options),
            std::make_shared<HROCGeneratorFactory>());
    }
};

class HPOMFactoryFeature : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    HPOMFactoryFeature()
        : SharedTypedFeature("hpom")
    {
        document_title("Projection Occupation Measure Heuristic");

        document_synopsis(
            "The projection occupation measure heuristic. "
            "For details, see" +
            format_conference_reference(
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

        add_log_options_to_feature(*this);
        add_lp_solver_option_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            get_log_arguments_from_options(options),
            get_lp_solver_arguments_from_options(options),
            std::make_shared<HPOMGeneratorFactory>());
    }
};

class HOHPOMFactoryFeature : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    HOHPOMFactoryFeature()
        : SharedTypedFeature("ho_hpom")
    {
        document_title("Higher-Order Projection Occupation Measure Heuristic");
        document_synopsis(
            "The projection occupation measure heuristic with general "
            "projections"
            "beyond atomic projections. "
            "For details, see" +
            format_conference_reference(
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

        add_log_options_to_feature(*this);
        add_lp_solver_option_to_feature(*this);

        add_option<int>("projection_size", "The size of the projections", "1");
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            get_log_arguments_from_options(options),
            get_lp_solver_arguments_from_options(options),
            std::make_shared<HigherOrderHPOMGeneratorFactory>(
                options.get<int>("projection_size")));
    }
};

class HPHOFactoryFeature : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    HPHOFactoryFeature()
        : SharedTypedFeature("pho")
    {
        document_title("Post-hoc Optimization Heuristic");

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        add_log_options_to_feature(*this);
        add_lp_solver_option_to_feature(*this);

        add_option<std::shared_ptr<probfd::pdbs::PatternCollectionGenerator>>(
            "patterns",
            "The pattern generator used to construct the PDB collection which "
            "is subject to post-hoc optimization.",
            "det_adapter(generator=systematic(pattern_max_size=2))");
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            get_log_arguments_from_options(options),
            get_lp_solver_arguments_from_options(options),
            std::make_shared<PHOGeneratorFactory>(
                options.get_shared<PatternCollectionGenerator>(
                    "patterns")));
    }
};
} // namespace

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<HROCFactoryFeature>();
    raw_registry.insert_feature_plugin<HPOMFactoryFeature>();
    raw_registry.insert_feature_plugin<HOHPOMFactoryFeature>();
    raw_registry.insert_feature_plugin<HPHOFactoryFeature>();
}

} // namespace probfd::cli::occupation_measures