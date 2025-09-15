#ifndef OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_H
#define OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_H

#include "downward/operator_counting/constraint_generator.h"

#include <memory>

namespace downward::lm_cut_heuristic {
class LandmarkCutLandmarks;
}

namespace downward::operator_counting {
class LMCutConstraints : public ConstraintGenerator {
    std::unique_ptr<lm_cut_heuristic::LandmarkCutLandmarks> landmark_generator;

public:
    LMCutConstraints();
    ~LMCutConstraints() override;

    void initialize_constraints(
        const SharedAbstractTask& task,
        lp::LinearProgram& lp) override;

    bool
    update_constraints(const State& state, lp::LPSolver& lp_solver) override;
};
} // namespace downward::operator_counting

#endif
