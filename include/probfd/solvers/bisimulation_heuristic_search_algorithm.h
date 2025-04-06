#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/policy_picker.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/mdp_algorithm.h"
#include "probfd/task_proxy.h"

#include "downward/utils/timer.h"

namespace probfd::solvers {

struct BisimulationTimer {
    double time = 0.0;
    unsigned states = 0;
    unsigned transitions = 0;

    void print(std::ostream& out) const;
};

class BisimulationBasedHeuristicSearchAlgorithm : public FDRMDPAlgorithm {
protected:
    using QState = bisimulation::QuotientState;
    using QAction = downward::OperatorID;

    const std::shared_ptr<ProbabilisticTask> task_;
    const std::shared_ptr<FDRCostFunction> task_cost_function_;

    const std::string algorithm_name_;

    std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm_;

    BisimulationTimer stats_;

public:
    BisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm);

    std::unique_ptr<PolicyType> compute_policy(
        FDRMDP&,
        FDREvaluator&,
        const downward::State&,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H