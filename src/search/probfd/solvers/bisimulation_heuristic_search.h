#ifndef MDPS_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H
#define MDPS_SOLVERS_BISIMULATION_HEURISTIC_SEARCH_H

#include "../../utils/timer.h"
#include "../analysis_objectives/analysis_objective.h"
#include "../bisimulation/bisimilar_state_space.h"
#include "../bisimulation/engine_interfaces.h"
#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/transition_sampler.h"
#include "../globals.h"
#include "../logging.h"
// #include "mdp_heuristic_search.h"

namespace probfd {
namespace solvers {

struct BisimulationTimer {
    utils::Timer timer;
    unsigned states = 0;
    unsigned extended_states = 0;
    unsigned transitions = 0;

    BisimulationTimer()
    {
        logging::out << "Building bisimulation..." << std::endl;
    }

    void print(std::ostream& out) const
    {
        out << "  Bisimulation time: " << timer << std::endl;
        out << "  Bisimilar states: " << states << " (" << extended_states
            << ")" << std::endl;
        out << "  Transitions in bisimulation: " << transitions << std::endl;
    }
};

class BisimulationBasedHeuristicSearchEngine
    : public engines::MDPEngineInterface<GlobalState> {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotient_system::QuotientAction<QAction>;

public:
    template <
        template <typename, typename, typename>
        class HS,
        typename DualValues,
        typename... Args>
    static BisimulationBasedHeuristicSearchEngine* Constructor(
        const DualValues&,
        const std::string& engine_name,
        StateRegistry* state_registry,
        HeuristicSearchConnector& con,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args... args)
    {
        auto* res = new BisimulationBasedHeuristicSearchEngine(
            engine_name,
            state_registry);

        res->engine_.reset(new HS<QState, QAction, DualValues>(
            &res->state_id_map,
            &res->action_id_map,
            res->reward.get(),
            g_analysis_objective->reward_bound(),
            res->tgen.get(),
            nullptr,
            &res->policy_,
            &res->new_state_handler_,
            res->heuristic_.get(),
            &con,
            &progress,
            interval,
            stable_policy,
            args...));

        return res;
    }

    virtual value_type::value_t solve(const GlobalState&) override
    {
        logging::out << "Running " << engine_name_ << "..." << std::endl;
        const value_type::value_t val = engine_->solve(bs->get_initial_state());
        stats.extended_states = bs->num_extended_states();
        return val;
    }

    virtual bool supports_error_bound() const override
    {
        return engine_->supports_error_bound();
    }

    virtual value_type::value_t get_error(const GlobalState&) override
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
        const std::string& engine_name,
        StateRegistry* state_registry)
        : engine_name_(engine_name)
        , bs(new bisimulation::BisimilarStateSpace(
              state_registry->get_initial_state(),
              g_step_bound,
              g_step_cost_type))
        , tgen(new TransitionGenerator<QAction>(bs.get()))
        , reward(new bisimulation::DefaultQuotientRewardFunction(
              bs.get(),
              g_analysis_objective->reward_bound()))
        , heuristic_(new bisimulation::DefaultQuotientStateEvaluator(
              bs.get(),
              g_analysis_objective->reward_bound(),
              g_analysis_objective->reward_bound().upper))
    {
        stats.timer.stop();
        stats.states = bs->num_bisimilar_states();
        stats.transitions = bs->num_transitions();

        logging::out << "Bisimulation built after " << stats.timer << std::endl;
        logging::out << "Bisimilar state space contains "
                     << bs->num_bisimilar_states() << " states and "
                     << bs->num_transitions() << " transitions." << std::endl;
        logging::out << std::endl;
    }

    const std::string engine_name_;

    BisimulationTimer stats;

    std::unique_ptr<bisimulation::BisimilarStateSpace> bs;

    StateIDMap<QState> state_id_map;
    ActionIDMap<QAction> action_id_map;
    std::unique_ptr<TransitionGenerator<QAction>> tgen;
    std::unique_ptr<bisimulation::QuotientRewardFunction> reward;
    NewStateHandler<QState> new_state_handler_;
    std::unique_ptr<StateEvaluator<QState>> heuristic_;
    PolicyPicker<QAction> policy_;

    std::unique_ptr<MDPEngineInterface<QState>> engine_;
};

class QBisimulationBasedHeuristicSearchEngine
    : public BisimulationBasedHeuristicSearchEngine {
    using QState = bisimulation::QuotientState;
    using QAction = bisimulation::QuotientAction;
    using QQAction = quotient_system::QuotientAction<QAction>;

public:
    template <
        template <typename, typename, typename>
        class HS,
        typename DualValues,
        typename... Args>
    static QBisimulationBasedHeuristicSearchEngine* QConstructor(
        const DualValues&,
        const std::string& engine_name,
        StateRegistry* state_registry,
        HeuristicSearchConnector& con,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args... args)
    {
        auto* res = new QBisimulationBasedHeuristicSearchEngine(
            engine_name,
            state_registry);

        res->engine_ = std::unique_ptr<MDPEngineInterface<QState>>(
            new HS<QState, QQAction, DualValues>(
                args...,
                res->quotient_.get(),
                nullptr,
                res->q_policy_tiebreaker_.get(),
                &res->new_state_handler_,
                res->heuristic_.get(),
                &con,
                &progress,
                interval,
                stable_policy,
                &res->state_id_map,
                res->q_action_id_map_.get(),
                res->reward.get(),
                res->q_reward_.get(),
                g_analysis_objective->reward_bound(),
                res->q_transition_gen_.get()));

        return res;
    }

    template <
        template <typename, typename, typename>
        class Fret,
        template <typename, typename, typename>
        class HS,
        typename DualValues,
        typename... Args>
    static QBisimulationBasedHeuristicSearchEngine* Constructor(
        const DualValues&,
        const std::string& engine_name,
        StateRegistry* state_registry,
        HeuristicSearchConnector& con,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args... args)
    {
        auto* res = new QBisimulationBasedHeuristicSearchEngine(
            engine_name,
            state_registry);

        auto* engine = new HS<QState, QQAction, DualValues>(
            &res->state_id_map,
            res->q_action_id_map_.get(),
            res->q_reward_.get(),
            g_analysis_objective->reward_bound(),
            res->q_transition_gen_.get(),
            nullptr,
            res->q_policy_tiebreaker_.get(),
            &res->new_state_handler_,
            res->heuristic_.get(),
            &con,
            &progress,
            interval,
            stable_policy,
            args...);

        res->engine2_.reset(engine);

        res->engine_.reset(new Fret<QState, QAction, DualValues>(
            &res->state_id_map,
            &res->action_id_map,
            res->reward.get(),
            g_analysis_objective->reward_bound(),
            res->tgen.get(),
            res->quotient_.get(),
            &progress,
            engine));

        return res;
    }

private:
    explicit QBisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name,
        StateRegistry* state_registry)
        : BisimulationBasedHeuristicSearchEngine(engine_name, state_registry)
        , quotient_(new quotient_system::QuotientSystem<QAction>(
              &action_id_map,
              tgen.get()))
        , q_reward_(new quotient_system::DefaultQuotientRewardFunction(
              quotient_.get(),
              reward.get()))
        , q_action_id_map_(new ActionIDMap<QQAction>(quotient_.get()))
        , q_transition_gen_(new TransitionGenerator<QQAction>(quotient_.get()))
        , q_policy_tiebreaker_(
              new PolicyPicker<quotient_system::QuotientAction<QAction>>())
    {
    }

    std::unique_ptr<quotient_system::QuotientSystem<QAction>> quotient_;

    std::unique_ptr<RewardFunction<QState, QQAction>> q_reward_;
    std::unique_ptr<ActionIDMap<QQAction>> q_action_id_map_;
    std::unique_ptr<TransitionGenerator<QQAction>> q_transition_gen_;

    std::unique_ptr<PolicyPicker<QQAction>> q_policy_tiebreaker_;

    std::unique_ptr<MDPEngineInterface<QState>> engine2_;
};

template <>
class MDPHeuristicSearch<std::true_type, std::false_type>
    : public MDPHeuristicSearchBase {
public:
    using MDPHeuristicSearchBase::add_options_to_parser;
    using MDPHeuristicSearchBase::MDPHeuristicSearchBase;

    virtual std::string get_engine_name() const override
    {
        return get_heuristic_search_name() + "(bisimulation)";
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<GlobalState>*
    heuristic_search_engine_factory(Args... args)
    {
        if (dual_bounds_) {
            return BisimulationBasedHeuristicSearchEngine::template Constructor<
                HS>(
                std::true_type(),
                this->get_heuristic_search_name(),
                this->get_state_registry(),
                this->connector_,
                this->progress_,
                this->interval_comparison_,
                this->stable_policy_,
                args...);
        } else {
            return BisimulationBasedHeuristicSearchEngine::template Constructor<
                HS>(
                std::false_type(),
                this->get_heuristic_search_name(),
                this->get_state_registry(),
                this->connector_,
                this->progress_,
                this->interval_comparison_,
                this->stable_policy_,
                args...);
        }
    }

protected:
    template <typename T>
    using WrappedType =
        typename Wrapper<std::true_type, std::false_type, T>::type;

    template <typename T>
    typename Wrapper<std::true_type, std::false_type, T>::type wrap(T t) const
    {
        return Wrapper<std::true_type, std::false_type, T>()(t);
    }

    template <typename T>
    typename Unwrapper<std::true_type, std::false_type, T>::type
    unwrap(T t) const
    {
        return Unwrapper<std::true_type, std::false_type, T>()(t);
    }
};

template <>
class MDPHeuristicSearch<std::true_type, std::true_type>
    : public MDPHeuristicSearchBase {
public:
    explicit MDPHeuristicSearch(const options::Options& opts)
        : MDPHeuristicSearchBase(opts)
        , fret_on_policy_(opts.get<bool>("fret_on_policy"))
    {
    }

    using MDPHeuristicSearchBase::add_options_to_parser;

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<GlobalState>*
    heuristic_search_engine_factory(Args... args)
    {
        if (this->dual_bounds_) {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    std::true_type,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    std::true_type,
                    HS>(args...);
            }
        } else {
            if (this->fret_on_policy_) {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETPi,
                    std::false_type,
                    HS>(args...);
            } else {
                return this->template heuristic_search_engine_factory_wrapper<
                    engines::fret::FRETV,
                    std::false_type,
                    HS>(args...);
            }
        }
    }

    template <
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<GlobalState>*
    quotient_heuristic_search_factory(Args... args)
    {
        if (dual_bounds_) {
            return QBisimulationBasedHeuristicSearchEngine::
                template QConstructor<HS>(
                    std::true_type(),
                    this->get_heuristic_search_name(),
                    this->get_state_registry(),
                    this->connector_,
                    this->progress_,
                    this->interval_comparison_,
                    this->stable_policy_,
                    args...);
        } else {
            return QBisimulationBasedHeuristicSearchEngine::
                template QConstructor<HS>(
                    std::false_type(),
                    this->get_heuristic_search_name(),
                    this->get_state_registry(),
                    this->connector_,
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
    using WrappedType =
        typename Wrapper<std::true_type, std::true_type, T>::type;

    template <typename T>
    typename Wrapper<std::true_type, std::true_type, T>::type wrap(T t) const
    {
        return Wrapper<std::true_type, std::true_type, T>()(t);
    }

    template <typename T>
    typename Unwrapper<std::true_type, std::true_type, T>::type
    unwrap(T t) const
    {
        return Unwrapper<std::true_type, std::true_type, T>()(t);
    }

private:
    template <
        template <typename, typename, typename>
        class Fret,
        typename DualValues,
        template <typename, typename, typename>
        class HS,
        typename... Args>
    engines::MDPEngineInterface<GlobalState>*
    heuristic_search_engine_factory_wrapper(Args... args)
    {
        return QBisimulationBasedHeuristicSearchEngine::
            template Constructor<Fret, HS>(
                DualValues(),
                this->get_heuristic_search_name(),
                this->get_state_registry(),
                this->connector_,
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