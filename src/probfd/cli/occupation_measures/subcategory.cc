#include "probfd/cli/occupation_measures/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

using downward::cli::lp::add_lp_solver_option_to_feature;

namespace {
class HROCFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          Verbosity,
          lp::LPSolverType> {
public:
    HROCFactoryFeature()
        : TypedFeature("hroc", &HROCFactoryFeature::func)
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

        const auto n = add_log_options_to_feature(*this, 0);
        add_lp_solver_option_to_feature(*this, n);
    }

    static std::shared_ptr<TaskHeuristicFactory>
    func(const Context&, Verbosity verbosity, lp::LPSolverType lp_solver_type)
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            verbosity,
            lp_solver_type,
            std::make_shared<HROCGeneratorFactory>());
    }
};

class HPOMFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          Verbosity,
          lp::LPSolverType> {
public:
    HPOMFactoryFeature()
        : TypedFeature("hpom", &HPOMFactoryFeature::func)
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

        const auto n = add_log_options_to_feature(*this, 0);
        add_lp_solver_option_to_feature(*this, n);
    }

    static std::shared_ptr<TaskHeuristicFactory>
    func(const Context&, Verbosity verbosity, lp::LPSolverType lp_solver_type)
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            verbosity,
            lp_solver_type,
            std::make_shared<HPOMGeneratorFactory>());
    }
};

class HOHPOMFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          Verbosity,
          lp::LPSolverType,
          int> {
public:
    HOHPOMFactoryFeature()
        : TypedFeature("ho_hpom", &HOHPOMFactoryFeature::func)
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

        const auto n = add_log_options_to_feature(*this, 0);
        const auto n2 = add_lp_solver_option_to_feature(*this, n);

        make_optional_argument_with_default(
            n + n2,
            "projection_size",
            "1",
            "The size of the projections");
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        const Context&,
        Verbosity verbosity,
        lp::LPSolverType lp_solver_type,
        int projection_size)
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            verbosity,
            lp_solver_type,
            std::make_shared<HigherOrderHPOMGeneratorFactory>(projection_size));
    }
};

class HPHOFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          Verbosity,
          lp::LPSolverType,
          std::shared_ptr<PatternCollectionGenerator>> {
public:
    HPHOFactoryFeature()
        : TypedFeature("pho", &HPHOFactoryFeature::func)
    {
        document_title("Post-hoc Optimization Heuristic");

        document_language_support("conditional effects", "not supported");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes");
        document_property("consistent", "yes");

        const auto n = add_log_options_to_feature(*this, 0);
        const auto n2 = add_lp_solver_option_to_feature(*this, n);

        make_optional_argument_with_default(
            n + n2,
            "patterns",
            "det_adapter(generator=systematic(pattern_max_size=2))",
            "The pattern generator used to construct the PDB collection which "
            "is subject to post-hoc optimization.");
    }

    static std::shared_ptr<TaskHeuristicFactory> func(
        const Context&,
        Verbosity verbosity,
        lp::LPSolverType lp_solver,
        std::shared_ptr<PatternCollectionGenerator> generator)
    {
        return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
            verbosity,
            lp_solver,
            std::make_shared<PHOGeneratorFactory>(std::move(generator)));
    }
};
} // namespace

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<HROCFactoryFeature>();
    n.insert_feature_plugin<HPOMFactoryFeature>();
    n.insert_feature_plugin<HOHPOMFactoryFeature>();
    n.insert_feature_plugin<HPHOFactoryFeature>();
}

} // namespace probfd::cli::occupation_measures