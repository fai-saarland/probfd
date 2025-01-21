#include "downward/heuristics/ff_heuristic.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"

#include "downward/task_transformation.h"

#include <cassert>

using namespace std;

namespace ff_heuristic {
// construction and destruction
FFHeuristic::FFHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : AdditiveHeuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
    , relaxed_plan(task_proxy.get_operators().size(), false)
{
    if (log.is_at_least_normal()) {
        log << "Initializing FF heuristic..." << endl;
    }
}

FFHeuristic::FFHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : FFHeuristic(
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

void FFHeuristic::mark_preferred_operators_and_relaxed_plan(
    const State& state,
    PropID goal_id)
{
    Proposition* goal = get_proposition(goal_id);
    if (!goal->marked) { // Only consider each subgoal once.
        goal->marked = true;
        OpID op_id = goal->reached_by;
        if (op_id != NO_OP) { // We have not yet chained back to a start node.
            UnaryOperator* unary_op = get_operator(op_id);
            bool is_preferred = true;
            for (PropID precond : get_preconditions(op_id)) {
                mark_preferred_operators_and_relaxed_plan(state, precond);
                if (get_proposition(precond)->reached_by != NO_OP) {
                    is_preferred = false;
                }
            }
            int operator_no = unary_op->operator_no;
            if (operator_no != -1) {
                // This is not an axiom.
                relaxed_plan[operator_no] = true;
                if (is_preferred) {
                    OperatorProxy op = task_proxy.get_operators()[operator_no];
                    assert(task_properties::is_applicable(op, state));
                    set_preferred(op);
                }
            }
        }
    }
}

int FFHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int h_add = compute_add_and_ff(state);
    if (h_add == DEAD_END) return h_add;

    // Collecting the relaxed plan also sets the preferred operators.
    for (PropID goal_id : goal_propositions)
        mark_preferred_operators_and_relaxed_plan(state, goal_id);

    int h_ff = 0;
    for (size_t op_no = 0; op_no < relaxed_plan.size(); ++op_no) {
        if (relaxed_plan[op_no]) {
            relaxed_plan[op_no] = false; // Clean up for next computation.
            h_ff += task_proxy.get_operators()[op_no].get_cost();
        }
    }
    return h_ff;
}

} // namespace ff_heuristic
