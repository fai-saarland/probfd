#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/successor_sampler.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/evaluators.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "downward/state_registry.h"

#include "downward/utils/timer.h"

namespace probfd::solvers {

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
protected:
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;

    using QQState = quotients::QuotientState<QState, QAction>;
    using QQAction = quotients::QuotientAction<QAction>;

    const std::shared_ptr<ProbabilisticTask> task_;
    const std::shared_ptr<FDRCostFunction> task_cost_function_;

    const std::string algorithm_name_;

    std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm_;

    bisimulation::BisimilarStateSpace state_space_;

    BisimulationTimer stats_;

public:
    explicit BisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm)
        : task_(std::move(task))
        , task_cost_function_(std::move(task_cost_function))
        , algorithm_name_(std::move(algorithm_name))
        , algorithm_(std::move(algorithm))
        , state_space_(
              task.get(),
              task_cost_function->get_non_goal_termination_cost())
    {
        stats_.timer.stop();
        stats_.states = state_space_.num_bisimilar_states();
        stats_.transitions = state_space_.num_transitions();

        std::cout << "Bisimulation built after " << stats_.timer << std::endl;
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

    Interval solve(
        FDRMDP&,
        FDREvaluator&,
        const State&,
        ProgressReport progress,
        double max_time) override
    {
        bisimulation::InducedQuotientEvaluator heuristic(
            &state_space_,
            task_cost_function_->get_non_goal_termination_cost());

        std::cout << "Running " << algorithm_name_ << "..." << std::endl;
        return algorithm_->solve(
            state_space_,
            heuristic,
            state_space_.get_initial_state(),
            progress,
            max_time);
    }

    void print_statistics(std::ostream& out) const override
    {
        stats_.print(out);

        out << std::endl;
        out << "Algorithm " << algorithm_name_ << " statistics:" << std::endl;
        algorithm_->print_statistics(out);
    }
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H