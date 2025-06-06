#ifndef OPERATOR_COUNTING_OPERATOR_COUNTING_HEURISTIC_H
#define OPERATOR_COUNTING_OPERATOR_COUNTING_HEURISTIC_H

#include "downward/heuristic.h"

#include "downward/lp/lp_solver.h"

#include <memory>
#include <vector>

namespace downward::operator_counting {
class ConstraintGenerator;

class OperatorCountingHeuristic : public Heuristic {
    std::vector<std::shared_ptr<ConstraintGenerator>> constraint_generators;
    lp::LPSolver lp_solver;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    OperatorCountingHeuristic(
        const std::vector<std::shared_ptr<ConstraintGenerator>>&
            constraint_generators,
        bool use_integer_operator_counts,
        lp::LPSolverType lpsolver,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    OperatorCountingHeuristic(
        const std::vector<std::shared_ptr<ConstraintGenerator>>&
            constraint_generators,
        bool use_integer_operator_counts,
        lp::LPSolverType lpsolver,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace operator_counting

#endif
