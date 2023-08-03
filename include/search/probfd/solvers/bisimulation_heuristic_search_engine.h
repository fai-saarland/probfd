#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/cost_model.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

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

class BisimulationBasedHeuristicSearchEngine : public TaskMDPEngine {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotients::QuotientAction<QAction>;

protected:
    const std::shared_ptr<ProbabilisticTask> task;

    const std::string engine_name_;

    BisimulationTimer stats;

    bisimulation::BisimilarStateSpace state_space_;

    std::shared_ptr<Evaluator<QState>> heuristic_;
    std::shared_ptr<engine_interfaces::PolicyPicker<QState, QAction>> policy_;
    std::shared_ptr<engine_interfaces::NewStateObserver<QState>>
        new_state_handler_;

    std::shared_ptr<MDPEngine<QState, QAction>> engine_;

public:
    explicit BisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : task(tasks::g_root_task)
        , engine_name_(engine_name)
        , state_space_(task.get(), g_cost_model->optimal_value_bound())
        , heuristic_(new bisimulation::InducedQuotientEvaluator(
              &state_space_,
              g_cost_model->optimal_value_bound(),
              g_cost_model->optimal_value_bound().upper))
        , policy_(
              new policy_pickers::ArbitraryTiebreaker<QState, QAction>(true))
        , new_state_handler_(new engine_interfaces::NewStateObserver<QState>())
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
    static std::unique_ptr<BisimulationBasedHeuristicSearchEngine> Constructor(
        const std::string& engine_name,
        ProgressReport& progress,
        bool interval,
        Args&&... args)
    {
        auto res = std::make_unique<BisimulationBasedHeuristicSearchEngine>(
            engine_name);

        res->engine_.reset(new HS<QState, QAction, Interval>(
            res->heuristic_.get(),
            res->policy_.get(),
            res->new_state_handler_.get(),
            &progress,
            interval,
            std::forward<Args>(args)...));

        return res;
    }

    virtual Interval solve(TaskMDP&, const State&, double max_time) override
    {
        std::cout << "Running " << engine_name_ << "..." << std::endl;
        return engine_->solve(
            state_space_,
            state_space_.get_initial_state(),
            max_time);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        stats.print(out);

        out << std::endl;
        out << "Engine " << engine_name_ << " statistics:" << std::endl;
        engine_->print_statistics(out);
    }
};

class QBisimulationBasedHeuristicSearchEngine
    : public BisimulationBasedHeuristicSearchEngine {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotients::QuotientAction<QAction>;

    std::shared_ptr<engine_interfaces::PolicyPicker<QState, QQAction>>
        q_policy_tiebreaker_;

public:
    explicit QBisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : BisimulationBasedHeuristicSearchEngine(engine_name)
        , q_policy_tiebreaker_(new policy_pickers::ArbitraryTiebreaker<
                               QState,
                               quotients::QuotientAction<QAction>>(true))
    {
    }

    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static std::unique_ptr<QBisimulationBasedHeuristicSearchEngine> Constructor(
        const std::string& engine_name,
        ProgressReport& progress,
        bool interval,
        Args&&... args)
    {
        auto res = std::make_unique<QBisimulationBasedHeuristicSearchEngine>(
            engine_name);

        std::shared_ptr<HS<QState, QQAction, Interval>> engine(
            new HS<QState, QQAction, Interval>(
                res->heuristic_.get(),
                res->q_policy_tiebreaker_.get(),
                res->new_state_handler_.get(),
                &progress,
                interval,
                std::forward<Args>(args)...));

        res->engine_.reset(
            new Fret<QState, QAction, Interval>(&progress, engine));

        return res;
    }
};

} // namespace solvers
} // namespace probfd

#endif // __BISIMULATION_HEURISTIC_SEARCH_H__