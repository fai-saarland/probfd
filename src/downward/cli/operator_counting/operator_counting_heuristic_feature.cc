#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/cli/lp/lp_solver_options.h"

#include "downward/operator_counting/operator_counting_heuristic.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

using downward::cli::lp::add_lp_solver_option_to_feature;
using downward::cli::lp::get_lp_solver_arguments_from_options;

namespace {

class OperatorCountingHeuristicFeature
    : public TypedFeature<downward::Evaluator, OperatorCountingHeuristic> {
public:
    OperatorCountingHeuristicFeature()
        : TypedFeature("operatorcounting")
    {
        document_title("Operator-counting heuristic");
        document_synopsis(
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

        add_list_option<shared_ptr<ConstraintGenerator>>(
            "constraint_generators",
            "methods that generate constraints over operator-counting "
            "variables");
        add_option<bool>(
            "use_integer_operator_counts",
            "restrict operator-counting variables to integer values. Computing "
            "the "
            "heuristic with integer variables can produce higher values but "
            "requires solving a MIP instead of an LP which is generally more "
            "computationally expensive. Turning this option on can thus "
            "drastically "
            "increase the runtime.",
            "false");
        add_lp_solver_option_to_feature(*this);
        add_heuristic_options_to_feature(*this, "operatorcounting");

        document_language_support("action costs", "supported");
        document_language_support(
            "conditional effects",
            "not supported (the heuristic supports them in theory, but none of "
            "the currently implemented constraint generators do)");
        document_language_support(
            "axioms",
            "not supported (the heuristic supports them in theory, but none of "
            "the currently implemented constraint generators do)");

        document_property("admissible", "yes");
        document_property(
            "consistent",
            "yes, if all constraint generators represent consistent "
            "heuristics");
        document_property("safe", "yes");
        // TODO: prefer operators that are non-zero in the solution.
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<OperatorCountingHeuristic>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<ConstraintGenerator>>(
            context,
            opts,
            "constraint_generators");
        return make_shared_from_arg_tuples<OperatorCountingHeuristic>(
            opts.get_list<shared_ptr<ConstraintGenerator>>(
                "constraint_generators"),
            opts.get<bool>("use_integer_operator_counts"),
            get_lp_solver_arguments_from_options(opts),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<OperatorCountingHeuristicFeature> _plugin;

} // namespace
