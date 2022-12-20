#ifndef OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_H
#define OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_H

#include "operator_counting/constraint_generator.h"

#include <set>

namespace lp {
class LPConstraint;
}

namespace operator_counting {
/* A proposition is an atom of the form Var = Val. It stores the index of the
   constraint representing it in the LP */
struct Proposition {
    int constraint_index;
    std::set<int> always_produced_by;
    std::set<int> sometimes_produced_by;
    std::set<int> always_consumed_by;

    Proposition()
        : constraint_index(-1)
    {
    }
    ~Proposition() = default;
};

class StateEquationConstraints : public ConstraintGenerator {
    std::vector<std::vector<Proposition>> propositions;
    // Map goal variables to their goal value and other variables to max int.
    std::vector<int> goal_state;

    void build_propositions();
    void add_constraints(
        named_vector::NamedVector<lp::LPConstraint>& constraints,
        double infinity);

public:
    virtual void initialize_constraints(
        OperatorCost cost_type,
        named_vector::NamedVector<lp::LPConstraint>& constraints,
        double infinity);
    virtual bool
    update_constraints(const GlobalState& state, lp::LPSolver& lp_solver);
};
} // namespace operator_counting

#endif
