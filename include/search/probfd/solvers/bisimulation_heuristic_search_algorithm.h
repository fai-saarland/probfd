#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/successor_sampler.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/bisimulation/algorithm_interfaces.h"
#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "downward/state_registry.h"

#include "downward/utils/timer.h"

namespace probfd {
namespace solvers {

struct BisimulationTimer {
    utils::Timer timer;
    unsigned states = 0;
    unsigned transitions = 0;

    BisimulationTimer()
    {
        std::cout << "Building bisimulation..." << std::endl;
    }

    void print(std::ostream& out) const
    {
        out << "  Bisimulation time: " << timer << std::endl;
        out << "  Bisimilar states: " << states << std::endl;
        out << "  Transitions in bisimulation: " << transitions << std::endl;
    }
};

class BisimulationBasedHeuristicSearchAlgorithm : public FDRMDPAlgorithm {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

protected:
    const std::shared_ptr<ProbabilisticTask> task;
    const std::shared_ptr<FDRCostFunction> task_cost_function;

    const std::string algorithm_name_;

    BisimulationTimer stats;

    bisimulation::BisimilarStateSpace state_space_;

    std::shared_ptr<algorithms::PolicyPicker<QState, QAction>> policy_;

    std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm_;

public:
    explicit BisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        const std::string& algorithm_name)
        : task(std::move(task))
        , task_cost_function(std::move(task_cost_function))
        , algorithm_name_(algorithm_name)
        , state_space_(
              task.get(),
              task_cost_function->get_non_goal_termination_cost())
        , policy_(
              new policy_pickers::ArbitraryTiebreaker<QState, QAction>(true))
    {
        stats.timer.stop();
        stats.states = state_space_.num_bisimilar_states();
        stats.transitions = state_space_.num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << state_space_.num_bisimilar_states() << " states and "
                  << state_space_.num_transitions() << " transitions."
                  << std::endl;
        std::cout << std::endl;
    }

    template <
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static std::unique_ptr<BisimulationBasedHeuristicSearchAlgorithm>
    Constructor(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        const std::string& algorithm_name,
        ProgressReport& progress,
        bool interval,
        Args&&... args)
    {
        auto res = std::make_unique<BisimulationBasedHeuristicSearchAlgorithm>(
            std::move(task),
            std::move(task_cost_function),
            algorithm_name);

        res->algorithm_.reset(new HS<QState, QAction, Interval>(
            res->policy_,
            &progress,
            interval,
            std::forward<Args>(args)...));

        return res;
    }

    virtual Interval
    solve(FDRMDP&, FDREvaluator&, const State&, double max_time) override
    {
        bisimulation::InducedQuotientEvaluator heuristic(
            &state_space_,
            task_cost_function->get_non_goal_termination_cost());

        std::cout << "Running " << algorithm_name_ << "..." << std::endl;
        return algorithm_->solve(
            state_space_,
            heuristic,
            state_space_.get_initial_state(),
            max_time);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        stats.print(out);

        out << std::endl;
        out << "Algorithm " << algorithm_name_ << " statistics:" << std::endl;
        algorithm_->print_statistics(out);
    }
};

class QBisimulationBasedHeuristicSearchAlgorithm
    : public BisimulationBasedHeuristicSearchAlgorithm {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

    std::shared_ptr<algorithms::PolicyPicker<QQState, QQAction>>
        q_policy_tiebreaker_;

public:
    explicit QBisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        const std::string& algorithm_name)
        : BisimulationBasedHeuristicSearchAlgorithm(
              std::move(task),
              std::move(task_cost_function),
              algorithm_name)
        , q_policy_tiebreaker_(
              new policy_pickers::ArbitraryTiebreaker<QQState, QQAction>(true))
    {
    }

    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static std::unique_ptr<QBisimulationBasedHeuristicSearchAlgorithm>
    Constructor(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        const std::string& algorithm_name,
        ProgressReport& progress,
        bool interval,
        Args&&... args)
    {
        auto res = std::make_unique<QBisimulationBasedHeuristicSearchAlgorithm>(
            std::move(task),
            std::move(task_cost_function),
            algorithm_name);

        std::shared_ptr<HS<QQState, QQAction, Interval>> algorithm(
            new HS<QQState, QQAction, Interval>(
                res->q_policy_tiebreaker_,
                &progress,
                interval,
                std::forward<Args>(args)...));

        res->algorithm_.reset(
            new Fret<QState, QAction, Interval>(&progress, algorithm));

        return res;
    }
};

} // namespace solvers
} // namespace probfd

#endif // __BISIMULATION_HEURISTIC_SEARCH_H__