#include "downward/cli/operator_counting/delete_relaxation_rr_constraints_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/operator_counting/delete_relaxation_rr_constraints.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace {
class DeleteRelaxationRRConstraintsFeature
    : public TypedFeature<ConstraintGenerator> {
public:
    DeleteRelaxationRRConstraintsFeature()
        : TypedFeature("delete_relaxation_rr_constraints")
    {
        document_title(
            "Delete relaxation constraints from Rankooh and Rintanen");
        document_synopsis(
            "Operator-counting constraints based on the delete relaxation. By "
            "default the constraints encode an easy-to-compute relaxation of "
            "h^+^. "
            "With the right settings, these constraints can be used to compute "
            "the "
            "optimal delete-relaxation heuristic h^+^ (see example below). "
            "For details, see" +
            format_journal_reference(
                {"Masood Feyzbakhsh Rankooh", "Jussi Rintanen"},
                "Efficient Computation and Informative Estimation of"
                "h+ by Integer and Linear Programming"
                "",
                "https://ojs.aaai.org/index.php/ICAPS/article/view/19787/19546",
                "Proceedings of the Thirty-Second International Conference on "
                "Automated Planning and Scheduling (ICAPS2022)",
                "32",
                "71-79",
                "2022"));

        add_option<AcyclicityType>(
            "acyclicity_type",
            "The most relaxed version of this constraint only enforces that "
            "achievers of facts are picked in such a way that all goal facts "
            "have an achiever, and the preconditions all achievers are either "
            "true in the current state or have achievers themselves. In this "
            "version, cycles in the achiever relation can occur. Such cycles "
            "can be excluded with additional auxilliary varibles and "
            "constraints.",
            "vertex_elimination");
        add_option<bool>(
            "use_integer_vars",
            "restrict auxiliary variables to integer values. These variables "
            "encode whether facts are reached, which operator first achieves "
            "which fact, and (depending on the acyclicity_type) in which order "
            "the operators are used. Restricting them to integers generally "
            "improves the heuristic value at the cost of increased runtime.",
            "false");

        document_note(
            "Example",
            "To compute the optimal delete-relaxation heuristic h^+^, use"
            "integer variables and some way of enforcing acyclicity (other "
            "than \"none\"). For example\n"
            "{{{\noperatorcounting([delete_relaxation_rr_constraints("
            "acyclicity_type=vertex_elimination, use_integer_vars=true)], "
            "use_integer_operator_counts=true))\n}}}\n");
        document_note(
            "Note",
            "While the delete-relaxation constraints by Imai and Fukunaga "
            "(accessible via option {{{delete_relaxation_if_constraints}}}) "
            "serve a similar purpose to the constraints implemented here, we "
            "recommend using this formulation as it can generally be solved "
            "more efficiently, in particular in case of the h^+^ "
            "configuration, and some relaxations offer tighter bounds.\n");
    }

    virtual std::shared_ptr<ConstraintGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<DeleteRelaxationRRConstraints>(
            opts.get<AcyclicityType>("acyclicity_type"),
            opts.get<bool>("use_integer_vars"));
    }
};
} // namespace

namespace downward::cli::operator_counting {

void add_delete_relaxation_rr_constraints_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<DeleteRelaxationRRConstraintsFeature>();

    raw_registry.insert_enum_plugin<AcyclicityType>(
        {{"time_labels",
          "introduces MIP variables that encode the time at which each fact is "
          "reached. Acyclicity is enforced with constraints that ensure that "
          "preconditions of actions are reached before their effects."},
         {"vertex_elimination",
          "introduces binary variables based on vertex elimination. These "
          "variables encode that one fact has to be reached before another "
          "fact. Instead of adding such variables for every pair of states, "
          "they are only added for a subset sufficient to ensure acyclicity. "
          "Constraints enforce that preconditions of actions are reached "
          "before "
          "their effects and that the assignment encodes a valid order."},
         {"none",
          "No acyclicity is enforced. The resulting heuristic is a relaxation "
          "of the delete-relaxation heuristic."}});
}

} // namespace downward::cli::operator_counting