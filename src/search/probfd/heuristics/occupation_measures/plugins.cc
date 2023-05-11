#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/heuristics/occupation_measures/higher_order_hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hpom_constraints.h"
#include "probfd/heuristics/occupation_measures/hroc_constraints.h"
#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"
#include "probfd/heuristics/occupation_measures/pho_constraints.h"

#include "utils/markup.h"

#include "option_parser.h"
#include "plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace occupation_measures {

static std::shared_ptr<TaskEvaluator> _parse_hroc(OptionParser& parser)
{
    parser.document_synopsis(
        "Regrouped operator-counting heuristic",
        "The regrouped operator-counting heuristic. "
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

    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");

    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");

    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);

    options::Options opts = parser.parse();

    if (parser.help_mode()) return nullptr;
    if (parser.dry_run()) return nullptr;

    opts.set<std::shared_ptr<ConstraintGenerator>>(
        "constraint_generator",
        std::make_shared<HROCGenerator>());

    return std::make_shared<OccupationMeasureHeuristic>(opts);
}

static std::shared_ptr<TaskEvaluator> _parse_hpom(OptionParser& parser)
{
    parser.document_synopsis(
        "Projection Occupation Measure Heuristic",
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

    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");

    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");

    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);

    options::Options opts = parser.parse();

    if (parser.help_mode()) return nullptr;
    if (parser.dry_run()) return nullptr;

    opts.set<std::shared_ptr<ConstraintGenerator>>(
        "constraint_generator",
        std::make_shared<HPOMGenerator>());

    return std::make_shared<OccupationMeasureHeuristic>(opts);
}

static std::shared_ptr<TaskEvaluator> _parse_hohpom(OptionParser& parser)
{
    parser.document_synopsis(
        "Higher-Order Projection Occupation Measure Heuristic",
        "The projection occupation measure heuristic with general projections"
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

    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");

    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");

    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);

    parser.add_option<int>(
        "projection_size",
        "The size of the projections",
        "1");

    options::Options opts = parser.parse();

    if (parser.help_mode()) return nullptr;
    if (parser.dry_run()) return nullptr;

    opts.set<std::shared_ptr<ConstraintGenerator>>(
        "constraint_generator",
        std::make_shared<HigherOrderHPOMGenerator>(opts));

    return std::make_shared<OccupationMeasureHeuristic>(opts);
}

static std::shared_ptr<TaskEvaluator> _parse_pho(OptionParser& parser)
{
    parser.document_synopsis(
        "Higher-Order Projection Occupation Measure Heuristic",
        "The projection occupation measure heuristic with general projections"
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

    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");

    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");

    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);

    parser.add_option<std::shared_ptr<pdbs::PatternCollectionGenerator>>(
        "patterns",
        "The pattern generator used to construct the PDB collection which is "
        "subject to post-hoc optimization.",
        "det_adapter(generator=systematic(pattern_max_size=2))");

    options::Options opts = parser.parse();

    if (parser.help_mode()) return nullptr;
    if (parser.dry_run()) return nullptr;

    opts.set<std::shared_ptr<ConstraintGenerator>>(
        "constraint_generator",
        std::make_shared<PHOGenerator>(opts));

    return std::make_shared<OccupationMeasureHeuristic>(opts);
}

static Plugin<TaskEvaluator> _plugin_hroc("hroc", _parse_hroc);
static Plugin<TaskEvaluator> _plugin_hpom("hpom", _parse_hpom);
static Plugin<TaskEvaluator> _plugin_ho_hpom("ho_hpom", _parse_hohpom);
static Plugin<TaskEvaluator> _plugin_pho("pho", _parse_pho);

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd