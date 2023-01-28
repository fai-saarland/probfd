#ifndef MDPS_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H
#define MDPS_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H

#include "probfd/solvers/mdp_solver.h"

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/reward_model.h"

#include "probfd/bisimulation/bisimilar_state_space.h"
#include "probfd/bisimulation/engine_interfaces.h"

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "state_registry.h"

#include "utils/timer.h"

// #include "mdp_heuristic_search.h"

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
        Args... args)
    {
        auto* res = new BisimulationBasedHeuristicSearchEngine(engine_name);

        res->engine_.reset(new HS<QState, QAction, Interval>(
            &res->state_id_map,
            &res->action_id_map,
            res->reward.get(),
            res->tgen.get(),
            res->policy_.get(),
            &res->new_state_handler_,
            res->heuristic_.get(),
            &progress,
            interval,
            stable_policy,
            args...));

        return res;
    }

    virtual value_t solve(const State&) override
    {
        std::cout << "Running " << engine_name_ << "..." << std::endl;
        const value_t val = engine_->solve(bs->get_initial_state());
        return val;
    }

    virtual std::optional<value_t> get_error(const State&) override
    {
        return engine_->get_error(bs->get_initial_state());
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        stats.print(out);

        out << std::endl;
        out << "Engine " << engine_name_ << " statistics:" << std::endl;
        engine_->print_statistics(out);
    }

protected:
    explicit BisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : task(tasks::g_root_task)
        , engine_name_(engine_name)
        , bs(new bisimulation::BisimilarStateSpace(task.get()))
        , tgen(new engine_interfaces::TransitionGenerator<QAction>(bs.get()))
        , reward(new bisimulation::DefaultQuotientRewardFunction(
              bs.get(),
              g_reward_model->reward_bound()))
        , heuristic_(new bisimulation::DefaultQuotientStateEvaluator(
              bs.get(),
              g_reward_model->reward_bound(),
              g_reward_model->reward_bound().upper))
        , policy_(new policy_pickers::ArbitraryTiebreaker<QAction>())
    {
        stats.timer.stop();
        stats.states = bs->num_bisimilar_states();
        stats.transitions = bs->num_transitions();

        std::cout << "Bisimulation built after " << stats.timer << std::endl;
        std::cout << "Bisimilar state space contains "
                  << bs->num_bisimilar_states() << " states and "
                  << bs->num_transitions() << " transitions." << std::endl;
        std::cout << std::endl;
    }

    const std::shared_ptr<ProbabilisticTask> task;

    const std::string engine_name_;

    BisimulationTimer stats;

    std::unique_ptr<bisimulation::BisimilarStateSpace> bs;

    engine_interfaces::StateIDMap<QState> state_id_map;
    engine_interfaces::ActionIDMap<QAction> action_id_map;
    std::unique_ptr<engine_interfaces::TransitionGenerator<QAction>> tgen;
    std::unique_ptr<bisimulation::QuotientRewardFunction> reward;
    engine_interfaces::NewStateHandler<QState> new_state_handler_;
    std::unique_ptr<engine_interfaces::StateEvaluator<QState>> heuristic_;
    std::unique_ptr<engine_interfaces::PolicyPicker<QAction>> policy_;

    std::unique_ptr<MDPEngineInterface<QState>> engine_;
};

class QBisimulationBasedHeuristicSearchEngine
    : public BisimulationBasedHeuristicSearchEngine {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotients::QuotientAction<QAction>;

public:
    template <
        template <typename, typename, bool>
        class HS,
        bool Interval,
        typename... Args>
    static QBisimulationBasedHeuristicSearchEngine* QConstructor(
        const std::string& engine_name,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args... args)
    {
        auto* res = new QBisimulationBasedHeuristicSearchEngine(engine_name);

        res->engine_ = std::unique_ptr<MDPEngineInterface<QState>>(
            new HS<QState, QQAction, Interval>(
                args...,
                res->quotient_.get(),
                res->q_policy_tiebreaker_.get(),
                &res->new_state_handler_,
                res->heuristic_.get(),
                &progress,
                interval,
                stable_policy,
                &res->state_id_map,
                res->q_action_id_map_.get(),
                res->reward.get(),
                res->q_reward_.get(),
                g_reward_model->reward_bound(),
                res->q_transition_gen_.get()));

        return res;
    }

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
        Args... args)
    {
        auto* res = new QBisimulationBasedHeuristicSearchEngine(engine_name);

        auto* engine = new HS<QState, QQAction, Interval>(
            &res->state_id_map,
            res->q_action_id_map_.get(),
            res->q_reward_.get(),
            res->q_transition_gen_.get(),
            res->q_policy_tiebreaker_.get(),
            &res->new_state_handler_,
            res->heuristic_.get(),
            &progress,
            interval,
            stable_policy,
            args...);

        res->engine2_.reset(engine);

        res->engine_.reset(new Fret<QState, QAction, Interval>(
            &res->state_id_map,
            &res->action_id_map,
            res->reward.get(),
            res->tgen.get(),
            res->quotient_.get(),
            &progress,
            engine));

        return res;
    }

private:
    explicit QBisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name)
        : BisimulationBasedHeuristicSearchEngine(engine_name)
        , quotient_(new quotients::QuotientSystem<QAction>(
              &action_id_map,
              tgen.get()))
        , q_reward_(new quotients::DefaultQuotientRewardFunction(
              quotient_.get(),
              reward.get()))
        , q_action_id_map_(
              new engine_interfaces::ActionIDMap<QQAction>(quotient_.get()))
        , q_transition_gen_(
              new engine_interfaces::TransitionGenerator<QQAction>(
                  quotient_.get()))
        , q_policy_tiebreaker_(new policy_pickers::ArbitraryTiebreaker<
                               quotients::QuotientAction<QAction>>())
    {
    }

    std::unique_ptr<quotients::QuotientSystem<QAction>> quotient_;

    std::unique_ptr<engine_interfaces::RewardFunction<QState, QQAction>>
        q_reward_;
    std::unique_ptr<engine_interfaces::ActionIDMap<QQAction>> q_action_id_map_;
    std::unique_ptr<engine_interfaces::TransitionGenerator<QQAction>>
        q_transition_gen_;

    std::unique_ptr<engine_interfaces::PolicyPicker<QQAction>>
        q_policy_tiebreaker_;

    std::unique_ptr<MDPEngineInterface<QState>> engine2_;
};

template <>
class MDPHeuristicSearch<true, false> : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::add_options_to_parser;
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    virtual std::string get_engine_name() const override
    {
        return get_heuristic_search_name() + "(bisimulation)";
    }

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngineInterface<State>*
    heuristic_search_engine_factory(Args... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    args...);
        } else {
            return BisimulationBasedHeuristicSearchEngine::
                template Constructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    args...);
        }
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<true, false, T>::type;

    template <typename T>
    typename Wrapper<true, false, T>::type wrap(T t) const
    {
        return Wrapper<true, false, T>()(t);
    }

    template <typename T>
    typename Unwrapper<true, false, T>::type unwrap(T t) const
    {
        return Unwrapper<true, false, T>()(t);
    }
};

template <>
class MDPHeuristicSearch<true, true> : public MDPHeuristicSearchBase {
public:
    explicit MDPHeuristicSearch(const options::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , fret_on_policy_(opts.get<bool>("fret_on_policy"))
    {
    }

    using MDPHeuristicSearchBase::add_options_to_parser;

    template <template <typename, typename, bool> class HS, typename... Args>
    engines::MDPEngineInterface<State>*
    heuristic_search_engine_factory(Args... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    true,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    true,
                    HS>(args...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    false,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    false,
                    HS>(args...);
            }
        }
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<State>*
    quotient_heuristic_search_factory(Args... args)
    {
        if (dual_bounds_) {
            return QBisimulationBasedHeuristicSearchEngine::
                template QConstructor<HS, true>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    args...);
        } else {
            return QBisimulationBasedHeuristicSearchEngine::
                template QConstructor<HS, false>(
                    this->get_heuristic_search_name(),
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    args...);
        }
    }

    virtual std::string get_engine_name() const override
    {
        std::ostringstream out;
        out << "fret" << (fret_on_policy_ ? "_pi" : "_v") << "("
            << this->get_heuristic_search_name() << "(bisimulation)"
            << ")";
        return out.str();
    }

protected:
    template <typename T>
    using WrappedType = typename Wrapper<true, true, T>::type;

    template <typename T>
    typename Wrapper<true, true, T>::type wrap(T t) const
    {
        return Wrapper<true, true, T>()(t);
    }

    template <typename T>
    typename Unwrapper<true, true, T>::type unwrap(T t) const
    {
        return Unwrapper<true, true, T>()(t);
    }

private:
    template <
        template <typename, typename, bool>
        class Fret,
        bool Interval,
        template <typename, typename, bool>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<State>*
    heuristic_search_engine_factory_wrapper(Args... args)
    {
        return QBisimulationBasedHeuristicSearchEngine::
            template Constructor<Fret, HS, Interval>(
                this->get_heuristic_search_name(),
                this->progress_,
                this->interval_comparison_,
                this->stable_policy_,
                args...);
    }

    const bool fret_on_policy_;
};

} // namespace solvers
} // namespace probfd

#endif // __BISIMULATION_HEURISTIC_SEARCH_H__