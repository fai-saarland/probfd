#include "downward/operator_counting/lm_cut_constraints.h"

#include "downward/heuristics/lm_cut_landmarks.h"

#include "downward/lp/lp_solver.h"

#include "downward/utils/markup.h"
#include "downward/utils/memory.h"

#include <cassert>

using namespace std;

namespace operator_counting {

LMCutConstraints::LMCutConstraints() = default;
LMCutConstraints::~LMCutConstraints() = default;

void LMCutConstraints::initialize_constraints(
    const shared_ptr<AbstractTask>& task,
    lp::LinearProgram&)
{
    TaskProxy task_proxy(*task);
    landmark_generator =
        std::make_unique<lm_cut_heuristic::LandmarkCutLandmarks>(task_proxy);
}

bool LMCutConstraints::update_constraints(
    const State& state,
    lp::LPSolver& lp_solver)
{
    assert(landmark_generator);
    named_vector::NamedVector<lp::LPConstraint> constraints;
    double infinity = lp_solver.get_infinity();

    bool dead_end = landmark_generator->compute_landmarks(
        state,
        nullptr,
        [&](const vector<int>& op_ids, int /*cost*/) {
            constraints.emplace_back(1.0, infinity);
            lp::LPConstraint& landmark_constraint = constraints.back();
            for (int op_id : op_ids) {
                landmark_constraint.insert(op_id, 1.0);
            }
        });

    if (dead_end) {
        return true;
    } else {
        lp_solver.add_temporary_constraints(constraints);
        return false;
    }
}

} // namespace operator_counting
