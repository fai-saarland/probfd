#ifndef PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H
#define PROBFD_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/cost_model.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "state_registry.h"

#include "utils/timer.h"

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

class BisimulationBasedHeuristicSearchEngine
    : public engines::MDPEngineInterface<State> {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotients::QuotientAction<QAction>;

protected:
    const std::shared_ptr<ProbabilisticTask> task;

    const std::string engine_name_;

    BisimulationTimer stats;

    bisimulation::BisimilarStateSpace bs;

    engine_interfaces::StateIDMap<QState> state_id_map;
    engine_interfaces::ActionIDMap<QAction> action_id_map;
    engine_interfaces::TransitionGenerator<QAction> tgen;

    std::shared_ptr<bisimulation::QuotientCostFunction> cost;
    std::shared_ptr<engine_interfaces::Evaluator<QState>> heuristic_;
    std::shared_ptr<engine_interfaces::PolicyPicker<QAction>> policy_;
    std::shared_ptr<engine_interfaces::NewStateHandler<QState>>
        new_state_handler_;

    std::shared_ptr<MDPEngineInterface<QState>> engine_;

protected:
    explicit BisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : task(tasks::g_root_task)
        , engine_name_(engine_name)
        , bs(task.get())
        , tgen(&bs)
        , cost(new bisimulation::DefaultQuotientCostFunction(
              &bs,
              g_cost_model->optimal_value_bound()))
        , heuristic_(new bisimulation::DefaultQuotientEvaluator(
              &bs,
              g_cost_model->optimal_value_bound(),
              g_cost_model->optimal_value_bound().upper))
        , policy_(new policy_pickers::ArbitraryTiebreaker<QAction>())
        , new_state_handler_(new engine_interfaces::NewStateHandler<QState>())
    {
        stats.timer.stop();
        stats.states = bs.num_bisimilar_states();
        stats.transitions = bs.num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << bs.num_bisimilar_states() << " states and "
                  << bs.num_transitions() << " transitions." << std::endl;
        std::cout << std::endl;
    }

public:
    template <
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static BisimulationBasedHeuristicSearchEngine* Constructor(
        const std::string& engine_name,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args&&... args)
    {
        auto* res = new BisimulationBasedHeuristicSearchEngine(engine_name);

        res->engine_.reset(new HS<QState, QAction, Interval>(
            &res->state_id_map,
            &res->action_id_map,
            &res->tgen,
            res->cost.get(),
            res->heuristic_.get(),
            res->policy_.get(),
            res->new_state_handler_.get(),
            &progress,
            interval,
            stable_policy,
            std::forward<Args>(args)...));

        return res;
    }

    virtual value_t solve(const State&) override
    {
        std::cout << "Running " << engine_name_ << "..." << std::endl;
        return engine_->solve(bs.get_initial_state());
    }

    virtual std::optional<value_t> get_error(const State&) override
    {
        return engine_->get_error(bs.get_initial_state());
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

    quotients::QuotientSystem<QAction> quotient_;

    engine_interfaces::ActionIDMap<QQAction> q_action_id_map_;
    engine_interfaces::TransitionGenerator<QQAction> q_transition_gen_;

    std::shared_ptr<engine_interfaces::CostFunction<QState, QQAction>> q_cost_;
    std::shared_ptr<engine_interfaces::PolicyPicker<QQAction>>
        q_policy_tiebreaker_;

    explicit QBisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : BisimulationBasedHeuristicSearchEngine(engine_name)
        , quotient_(&action_id_map, &tgen)
        , q_action_id_map_(&quotient_)
        , q_transition_gen_(&quotient_)
        , q_cost_(new quotients::DefaultQuotientCostFunction(
              &quotient_,
              cost.get()))
        , q_policy_tiebreaker_(new policy_pickers::ArbitraryTiebreaker<
                               quotients::QuotientAction<QAction>>())
    {
    }

public:
    template <
        template <typename, typename, bool>
        class Fret,
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static QBisimulationBasedHeuristicSearchEngine* Constructor(
        const std::string& engine_name,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args&&... args)
    {
        auto* res = new QBisimulationBasedHeuristicSearchEngine(engine_name);

        std::shared_ptr<HS<QState, QQAction, Interval>> engine(
            new HS<QState, QQAction, Interval>(
                &res->state_id_map,
                &res->q_action_id_map_,
                &res->q_transition_gen_,
                res->q_cost_.get(),
                res->heuristic_.get(),
                res->q_policy_tiebreaker_.get(),
                res->new_state_handler_.get(),
                &progress,
                interval,
                stable_policy,
                std::forward<Args>(args)...));

        res->engine_.reset(new Fret<QState, QAction, Interval>(
            &res->state_id_map,
            &res->action_id_map,
            &res->tgen,
            res->cost.get(),
            &res->quotient_,
            &progress,
            engine));

        return res;
    }
};

} // namespace solvers
} // namespace probfd

#endif // __BISIMULATION_HEURISTIC_SEARCH_H__