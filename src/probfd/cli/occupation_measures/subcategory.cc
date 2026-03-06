#include "probfd/cli/occupation_measures/subcategory.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

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

using namespace language::parser;

using downward::cli::utils::add_log_options_to_feature;

using downward::cli::lp::add_lp_solver_option_to_feature;

namespace {

std::shared_ptr<TaskHeuristicFactory>
create_hroc(Verbosity verbosity, lp::LPSolverType lp_solver_type)
{
    return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
        verbosity,
        lp_solver_type,
        std::make_shared<HROCGeneratorFactory>());
}

std::shared_ptr<TaskHeuristicFactory>
create_hpom(Verbosity verbosity, lp::LPSolverType lp_solver_type)
{
    return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
        verbosity,
        lp_solver_type,
        std::make_shared<HPOMGeneratorFactory>());
}

std::shared_ptr<TaskHeuristicFactory> create_higher_order_hpom_heuristic(
    Verbosity verbosity,
    lp::LPSolverType lp_solver_type,
    int projection_size)
{
    return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
        verbosity,
        lp_solver_type,
        std::make_shared<HigherOrderHPOMGeneratorFactory>(projection_size));
}

std::shared_ptr<TaskHeuristicFactory> create_pho_heuristic(
    Verbosity verbosity,
    lp::LPSolverType lp_solver,
    std::shared_ptr<PatternCollectionGenerator> generator)
{
    return make_shared_from_arg_tuples<OccupationMeasureHeuristicFactory>(
        verbosity,
        lp_solver,
        std::make_shared<PHOGeneratorFactory>(std::move(generator)));
}

InternalFunctionDefinitionBase&
add_hroc_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_hroc>(nspace, "hroc");
    f.document_title("Regrouped operator-counting heuristic");
    f.document_synopsis(
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

    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");

    const auto n = add_log_options_to_feature(f, 0);
    add_lp_solver_option_to_feature(f, n);

    return f;
}

InternalFunctionDefinitionBase&
add_hpom_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_hpom>(nspace, "hpom");

    f.document_title("Projection Occupation Measure Heuristic");

    f.document_synopsis(
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

    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");

    const auto n = add_log_options_to_feature(f, 0);
    add_lp_solver_option_to_feature(f, n);

    return f;
}

InternalFunctionDefinitionBase& add_higher_order_hpom_heuristic_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_higher_order_hpom_heuristic>(
        nspace,
        "ho_hpom");
    f.document_title("Higher-Order Projection Occupation Measure Heuristic");
    f.document_synopsis(
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

    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");

    const auto n = add_log_options_to_feature(f, 0);
    const auto n2 = add_lp_solver_option_to_feature(f, n);

    f.make_required_argument(
        n + n2,
        "projection_size",
        "The size of the projections");

    return f;
}

InternalFunctionDefinitionBase&
add_hpho_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_pho_heuristic>(nspace, "hpho");
    f.document_title("Post-hoc Optimization Heuristic");

    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");

    const auto n = add_log_options_to_feature(f, 0);
    const auto n2 = add_lp_solver_option_to_feature(f, n);

    f.make_required_argument(
        n + n2,
        "patterns",
        "The pattern generator used to construct the PDB collection which "
        "is subject to post-hoc optimization.");

    return f;
}

} // namespace

namespace probfd::cli::occupation_measures {

void add_occupation_measure_heuristics_features(
    NamespaceLevelDeclarationList& nspace)
{
    add_hroc_to_namespace(nspace);
    add_hpom_to_namespace(nspace);
    add_higher_order_hpom_heuristic_to_namespace(nspace);
    add_hpho_to_namespace(nspace);
}

} // namespace probfd::cli::occupation_measures