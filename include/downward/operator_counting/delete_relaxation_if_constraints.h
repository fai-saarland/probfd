#ifndef OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_H
#define OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_H

#include "downward/operator_counting/constraint_generator.h"

#include "downward/state.h"

#include <memory>

namespace downward {
class OperatorProxy;
}

namespace downward::lp {
class LPConstraint;
struct LPVariable;
} // namespace downward::lp

namespace downward::operator_counting {
using LPConstraints = named_vector::NamedVector<lp::LPConstraint>;
using LPVariables = named_vector::NamedVector<lp::LPVariable>;

class DeleteRelaxationIFConstraints : public ConstraintGenerator {
    bool use_time_vars;
    bool use_integer_vars;

    /* [U_o] Is op part of the relaxed plan?
       Binary, indexed with op.id */
    std::vector<int> lp_var_id_op_used;

    /* [R_f] Is fact <V,v> reached by the relaxed plan?
       Binary, indexed with var.id, value */
    std::vector<std::vector<int>> lp_var_id_fact_reached;

    /* [F_{o,f}] Is o the first achiever of fact <V,v> in the relaxed plan?
       Binary, indexed with op.id, var.id, value */
    std::vector<std::vector<std::vector<int>>> lp_var_id_first_achiever;

    /* [T_o] At what time is o used first?
       {0, ..., |O|}, indexed with op.id */
    std::vector<int> lp_var_id_op_time;

    /* [T_f] At what time is <V,v> first achieved?
       {0, ..., |O|}, indexed with var.id, value */
    std::vector<std::vector<int>> lp_var_id_fact_time;

    /* Indices of constraints that change in every state
       Indexed with var.id, value */
    std::vector<std::vector<int>> constraint_ids;

    /* The state that is currently used for setting the bounds. Remembering
       this makes it faster to unset the bounds when the state changes. */
    std::vector<FactPair> last_state;

    int get_var_op_used(const OperatorProxy& op) const;
    int get_var_fact_reached(FactPair f) const;
    int get_var_first_achiever(const OperatorProxy& op, FactPair f) const;
    int get_var_op_time(const OperatorProxy& op) const;
    int get_var_fact_time(FactPair f) const;
    int get_constraint_id(FactPair f) const;

    void create_auxiliary_variables(
        const VariableSpace& variables,
        const ClassicalOperatorSpace& operators,
        LPVariables& lp_variables);

    void create_constraints(
        const VariableSpace& variables,
        const ClassicalOperatorSpace& operators,
        const GoalFactList& goals,
        lp::LinearProgram& lp);

public:
    explicit DeleteRelaxationIFConstraints(
        bool use_time_vars,
        bool use_integer_vars);

    void initialize_constraints(
        const SharedAbstractTask& task,
        lp::LinearProgram& lp) override;

    bool
    update_constraints(const State& state, lp::LPSolver& lp_solver) override;
};
} // namespace downward::operator_counting

#endif
