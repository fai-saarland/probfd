#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_ALGORITHM_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/successor_sampler.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/tasks/determinization_task.h"
#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "downward/state_registry.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/timer.h"

namespace probfd::solvers {

struct BisimulationTimer {
    double time;
    unsigned states = 0;
    unsigned transitions = 0;

    BisimulationTimer()
    {
        std::cout << "Building bisimulation..." << std::endl;
    }

    void print(std::ostream& out) const
    {
        out << "  Bisimulation time: " << time << std::endl;
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

    BisimulationTimer stats_;

public:
    BisimulationBasedHeuristicSearchAlgorithm(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::string algorithm_name,
        std::shared_ptr<MDPAlgorithm<QState, QAction>> algorithm)
        : task_(std::move(task))
        , task_cost_function_(std::move(task_cost_function))
        , algorithm_name_(std::move(algorithm_name))
        , algorithm_(std::move(algorithm))
    {
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
        utils::Timer timer;

        ProbabilisticTaskProxy task_proxy(*task_);

        std::shared_ptr determinization =
            std::make_shared<tasks::DeterminizationTask>(task_);

        TaskProxy det_task_proxy(*determinization);

        auto [transition_system, state_mapping, distances] =
            bisimulation::compute_bisimulation_on_determinization(
                det_task_proxy);

        if (!transition_system->is_solvable(*distances)) {
            std::cout << "Initial state recognized as unsolvable!" << std::endl;
            return Interval(1_vt, 1_vt);
        }

        State initial = task_proxy.get_initial_state();
        initial.unpack();
        const auto initial_state =
            bisimulation::QuotientState(state_mapping->get_value(initial));

        bisimulation::BisimilarStateSpace state_space(
            task_,
            det_task_proxy,
            *transition_system,
            1_vt);

        stats_.time = timer();
        stats_.states = state_space.num_bisimilar_states();
        stats_.transitions = state_space.num_transitions();

        std::cout << "Bisimulation built after " << stats_.time << std::endl;
        std::cout << "Bisimilar state space contains "
                  << state_space.num_bisimilar_states() << " states and "
                  << state_space.num_transitions() << " transitions."
                  << std::endl;
        std::cout << std::endl;
        
        heuristics::BlindEvaluator<QState> heuristic;

        std::cout << "Running " << algorithm_name_ << "..." << std::endl;
        return algorithm_
            ->solve(state_space, heuristic, initial_state, progress, max_time);
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