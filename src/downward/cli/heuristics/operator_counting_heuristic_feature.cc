#include "downward/cli/heuristics/operator_counting_heuristic_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "downward/operator_counting/operator_counting_heuristic.h"

#include "downward/utils/markup.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace language::plugins;

using downward::cli::add_heuristic_options_to_feature;

using downward::cli::lp::add_lp_solver_option_to_feature;

namespace {
class OperatorCountingHeuristicFactory
    : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::vector<std::shared_ptr<ConstraintGenerator>> constraint_generators;
    bool use_integer_operator_counts;
    lp::LPSolverType lp_solver;

public:
    OperatorCountingHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<ConstraintGenerator>> constraint_generators,
        bool use_integer_operator_counts,
        lp::LPSolverType lp_solver)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , constraint_generators(std::move(constraint_generators))
        , use_integer_operator_counts(use_integer_operator_counts)
        , lp_solver(lp_solver)
    {
        if (this->constraint_generators.empty()) {
            throw std::domain_error(
                "List of constraint generators may not be empty.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<OperatorCountingHeuristic>(
            constraint_generators,
            use_integer_operator_counts,
            lp_solver,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

} // namespace

namespace downward::cli::heuristics {

InternalFunctionDefinitionBase&
add_operator_counting_heuristic_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "operator_counting",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            OperatorCountingHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            std::vector<std::shared_ptr<ConstraintGenerator>>,
            bool,
            downward::lp::LPSolverType>);

    f.document_title("Operator-counting heuristic");
    f.document_synopsis(
        "An operator-counting heuristic computes a linear program (LP) in "
        "each "
        "state. The LP has one variable Count_o for each operator o that "
        "represents how often the operator is used in a plan. Operator-"
        "counting constraints are linear constraints over these varaibles "
        "that "
        "are guaranteed to have a solution with Count_o = occurrences(o, "
        "pi) "
        "for every plan pi. Minimizing the total cost of operators subject "
        "to "
        "some operator-counting constraints is an admissible heuristic. "
        "For details, see" +
        format_conference_reference(
            {"Florian Pommerening",
             "Gabriele Roeger",
             "Malte Helmert",
             "Blai Bonet"},
            "LP-based Heuristics for Cost-optimal Planning",
            "http://www.aaai.org/ocs/index.php/ICAPS/ICAPS14/paper/view/"
            "7892/8031",
            "Proceedings of the Twenty-Fourth International Conference"
            " on Automated Planning and Scheduling (ICAPS 2014)",
            "226-234",
            "AAAI Press",
            "2014"));

    f.document_language_support("action costs", "supported");
    f.document_language_support(
        "conditional effects",
        "not supported (the heuristic supports them in theory, but none of "
        "the currently implemented constraint generators do)");
    f.document_language_support(
        "axioms",
        "not supported (the heuristic supports them in theory, but none of "
        "the currently implemented constraint generators do)");

    f.document_property("admissible", "yes");
    f.document_property(
        "consistent",
        "yes, if all constraint generators represent consistent "
        "heuristics");
    f.document_property("safe", "yes");
    // TODO: prefer operators that are non-zero in the solution.
    f.document_property("preferred operators", "no");

    f.make_required_argument(
        0,
        "constraint_generators",
        "methods that generate constraints over operator-counting "
        "variables");
    f.make_optional_argument_with_default(
        1,
        "use_integer_operator_counts",
        "false",
        "restrict operator-counting variables to integer values. Computing "
        "the "
        "heuristic with integer variables can produce higher values but "
        "requires solving a MIP instead of an LP which is generally more "
        "computationally expensive. Turning this option on can thus "
        "drastically "
        "increase the runtime.");
    const auto n = add_lp_solver_option_to_feature(f, 2);
    add_heuristic_options_to_feature(f, "operatorcounting", n + 2);

    return f;
}

} // namespace downward::cli::heuristics
