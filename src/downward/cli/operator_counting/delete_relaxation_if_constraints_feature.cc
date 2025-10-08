#include "downward/cli/operator_counting/delete_relaxation_if_constraints_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/operator_counting/delete_relaxation_if_constraints.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/markup.h"

using namespace std;
using namespace downward::utils;
using namespace downward::operator_counting;

using namespace downward::cli::plugins;

namespace {
class DeleteRelaxationIFConstraintsFeature
    : public SharedTypedFeature<ConstraintGenerator> {
public:
    DeleteRelaxationIFConstraintsFeature()
        : SharedTypedFeature("delete_relaxation_if_constraints")
    {
        document_title("Delete relaxation constraints");
        document_synopsis(
            "Operator-counting constraints based on the delete relaxation. By "
            "default the constraints encode an easy-to-compute relaxation of "
            "h^+^. "
            "With the right settings, these constraints can be used to compute "
            "the "
            "optimal delete-relaxation heuristic h^+^ (see example below). "
            "For details, see" +
            format_journal_reference(
                {"Tatsuya Imai", "Alex Fukunaga"},
                "On a practical, integer-linear programming model for "
                "delete-free"
                "tasks and its use as a heuristic for cost-optimal planning",
                "https://www.jair.org/index.php/jair/article/download/10972/"
                "26119/",
                "Journal of Artificial Intelligence Research",
                "54",
                "631-677",
                "2015"));

        add_option<bool>(
            "use_time_vars",
            "use variables for time steps. With these additional variables the "
            "constraints enforce an order between the selected operators. "
            "Leaving "
            "this off (default) corresponds to the time relaxation by Imai and "
            "Fukunaga. Switching it on, can increase the heuristic value but "
            "will "
            "increase the size of the constraints which has a strong impact on "
            "runtime. Constraints involving time variables use a big-M "
            "encoding, "
            "so they are more useful if used with integer variables.",
            "false");
        add_option<bool>(
            "use_integer_vars",
            "restrict auxiliary variables to integer values. These variables "
            "encode whether operators are used, facts are reached, which "
            "operator "
            "first achieves which fact, and in which order the operators are "
            "used. "
            "Restricting them to integers generally improves the heuristic "
            "value "
            "at the cost of increased runtime.",
            "false");

        document_note(
            "Example",
            "To compute the optimal delete-relaxation heuristic h^+^, use\n"
            "{{{\noperatorcounting([delete_relaxation_if_constraints(use_time_"
            "vars=true, "
            "use_integer_vars=true)], "
            "use_integer_operator_counts=true))\n}}}\n");
        document_note(
            "Note",
            "For best performance we recommend using the alternative "
            "formulation by Rankooh and Rintanen, accessible through the "
            "option {{{delete_relaxation_rr_constraints}}}.\n");
    }

    virtual shared_ptr<ConstraintGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared<DeleteRelaxationIFConstraints>(
            opts.get<bool>("use_time_vars"),
            opts.get<bool>("use_integer_vars"));
    }
};
} // namespace

namespace downward::cli::operator_counting {

void add_delete_relaxation_if_constraints_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<DeleteRelaxationIFConstraintsFeature>();
}

} // namespace downward::cli::operator_counting
