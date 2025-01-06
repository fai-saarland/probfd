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
    using QAction = bisimulation::QuotientAction;

    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

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

    Interval solve(
        FDRMDP&,
        FDREvaluator&,
        const State&,
        ProgressReport progress,
        double max_time) override;

    std::unique_ptr<PolicyType> compute_policy(
        FDRMDP&,
        FDREvaluator&,
        const State&,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    template <
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static std::unique_ptr<BisimulationBasedHeuristicSearchAlgorithm> create(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<algorithms::PolicyPicker<QState, QAction>> tiebreaker,
        Args&&... args)
    {
        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            std::move(task),
            std::move(task_cost_function),
            std::move(algorithm_name),
            std::make_shared<HS<QState, QAction, Interval>>(
                tiebreaker,
                std::forward<Args>(args)...));
    }

    template <
        template <typename, typename, typename>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static std::unique_ptr<BisimulationBasedHeuristicSearchAlgorithm> create(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<algorithms::PolicyPicker<QQState, QQAction>> tiebreaker,
        Args&&... args)
    {
        using StateInfoT = typename HS<QQState, QQAction, Interval>::StateInfo;
        return std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            std::move(task),
            std::move(task_cost_function),
            std::move(algorithm_name),
            std::make_shared<Fret<QState, QAction, StateInfoT>>(
                std::make_shared<HS<QQState, QQAction, Interval>>(
                    tiebreaker,
                    std::forward<Args>(args)...)));
    }
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H