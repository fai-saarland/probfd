#include "downward/operator_counting/pho_constraints.h"

#include "downward/lp/lp_solver.h"

#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/utils.h"

#include "downward/abstract_task.h"
#include "downward/classical_operator_space.h"
#include "downward/operator_cost_function.h"
#include "downward/operator_cost_function_fwd.h"

#include <cassert>
#include <limits>
#include <memory>
#include <vector>

using namespace std;

namespace downward::operator_counting {

PhOConstraints::PhOConstraints(
    const shared_ptr<pdbs::PatternCollectionGenerator>& patterns)
    : pattern_generator(patterns)
{
}

void PhOConstraints::initialize_constraints(
    const SharedAbstractTask& task,
    lp::LinearProgram& lp)
{
    const auto& [operators, cost_function] = to_refs(
        slice_shared<ClassicalOperatorSpace, OperatorIntCostFunction>(task));

    assert(pattern_generator);
    pdbs::PatternCollectionInformation pattern_collection_info =
        pattern_generator->generate(task);
    /*
      TODO issue590: Currently initialize_constraints should only be called
      once. When we separate constraint generators from constraints, we can
      create pattern_generator locally and no longer need to explicitly reset
      it.
    */
    pdbs = pattern_collection_info.get_pdbs();
    pattern_generator = nullptr;
    named_vector::NamedVector<lp::LPConstraint>& constraints =
        lp.get_constraints();
    constraint_offset = constraints.size();

    for (const shared_ptr<pdbs::PatternDatabase>& pdb : *pdbs) {
        constraints.emplace_back(0, lp.get_infinity());
        lp::LPConstraint& constraint = constraints.back();
        for (OperatorProxy op : operators) {
            if (pdb->is_operator_relevant(op)) {
                constraint.insert(
                    op.get_id(),
                    cost_function.get_operator_cost(op.get_id()));
            }
        }
    }
}

bool PhOConstraints::update_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    state.unpack();
    for (size_t i = 0; i < pdbs->size(); ++i) {
        int constraint_id = constraint_offset + i;
        shared_ptr<pdbs::PatternDatabase> pdb = (*pdbs)[i];
        int h = pdb->get_value(state.get_unpacked_values());
        if (h == numeric_limits<int>::max()) { return true; }
        lp_solver.set_constraint_lower_bound(constraint_id, h);
    }
    return false;
}

} // namespace downward::operator_counting
