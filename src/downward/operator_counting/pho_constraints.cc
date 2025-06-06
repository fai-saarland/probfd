#include "downward/operator_counting/pho_constraints.h"

#include "downward/lp/lp_solver.h"
#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/utils.h"

#include "downward/utils/markup.h"

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
    const shared_ptr<AbstractTask>& task,
    lp::LinearProgram& lp)
{
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
    TaskProxy task_proxy(*task);
    named_vector::NamedVector<lp::LPConstraint>& constraints =
        lp.get_constraints();
    constraint_offset = constraints.size();
    for (const shared_ptr<pdbs::PatternDatabase>& pdb : *pdbs) {
        constraints.emplace_back(0, lp.get_infinity());
        lp::LPConstraint& constraint = constraints.back();
        for (OperatorProxy op : task_proxy.get_operators()) {
            if (pdb->is_operator_relevant(op)) {
                constraint.insert(op.get_id(), op.get_cost());
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
        if (h == numeric_limits<int>::max()) {
            return true;
        }
        lp_solver.set_constraint_lower_bound(constraint_id, h);
    }
    return false;
}

} // namespace operator_counting
