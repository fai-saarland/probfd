#include "probfd/heuristics/occupation_measures/occupation_measure_heuristic.h"

#include "probfd/heuristics/occupation_measures/constraint_generator.h"

#include "utils/markup.h"

#include "option_parser.h"
#include "plugin.h"

#include <cmath>

using namespace std;
using namespace options;

namespace probfd {
namespace heuristics {
namespace occupation_measures {

OccupationMeasureHeuristic::OccupationMeasureHeuristic(const Options& opts)
    : OccupationMeasureHeuristic(
          opts.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          opts.get<lp::LPSolverType>("lpsolver"),
          opts.get<shared_ptr<ConstraintGenerator>>("constraint_generator"))
{
}

OccupationMeasureHeuristic::OccupationMeasureHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    lp::LPSolverType solver_type,
    std::shared_ptr<ConstraintGenerator> constraint_generator)
    : LPHeuristic(task, solver_type)
    , constraint_generator_(constraint_generator)
{
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MINIMIZE,
        named_vector::NamedVector<lp::LPVariable>(),
        named_vector::NamedVector<lp::LPConstraint>(),
        lp_solver_.get_infinity());

    constraint_generator_->initialize_constraints(task, lp);

    lp_solver_.load_problem(lp);
}

void OccupationMeasureHeuristic::update_constraints(const State& state) const
{
    constraint_generator_->update_constraints(state, lp_solver_);
}

void OccupationMeasureHeuristic::reset_constraints(const State& state) const
{
    constraint_generator_->reset_constraints(state, lp_solver_);
}

static shared_ptr<TaskEvaluator> _parse(OptionParser& parser)
{
    parser.document_synopsis(
        "Occupation measure heuristic",
        "An occupation measure heuristic is the generalization of an operator"
        "counting heuristic for stochastic shortest path problems. It computes"
        "a linear program (LP) in each state. The LP has one variable Y_o for"
        "each operator o that represents how often the operator is used in"
        "expectation, i.e., the occupation measure of the operator. Occupation"
        "measure constraints are linear constraints over these varaibles that "
        "are guaranteed to have a solution with Y_o = occmeasure(o, pi)"
        "for every policy pi. Minimizing the total expected cost of operators,"
        "i.e., the cost-weighted sum of the occupation measures, subject to "
        "some occupation measure constraints is an admissible heuristic. "
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
    parser.document_property(
        "consistent",
        "yes, if all constraint generators represent consistent heuristics");

    parser.add_option<shared_ptr<ConstraintGenerator>>(
        "constraint_generator",
        "method that generates the constraints over occupation measure "
        "variables");

    TaskDependentHeuristic::add_options_to_parser(parser);
    lp::add_lp_solver_option_to_parser(parser);

    Options opts = parser.parse();
    if (parser.help_mode()) return nullptr;
    if (parser.dry_run()) return nullptr;
    return std::make_shared<OccupationMeasureHeuristic>(opts);
}

static Plugin<TaskEvaluator> _plugin("occupation_measure_heuristic", _parse);

} // namespace occupation_measures
} // namespace heuristics
} // namespace probfd