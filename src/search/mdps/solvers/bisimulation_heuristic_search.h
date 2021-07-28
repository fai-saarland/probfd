#pragma once

#include "../../utils/timer.h"
#include "../analysis_objectives/analysis_objective.h"
#include "../bisimulation/bisimilar_state_space.h"
#include "../bisimulation/engine_interfaces.h"
#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/transition_sampler.h"
#include "../globals.h"
#include "../logging.h"
// #include "mdp_heuristic_search.h"

namespace probabilistic {
namespace solvers {

struct BisimulationTimer {
    utils::Timer timer;
    unsigned states = 0;
    unsigned extended_states = 0;
    unsigned transitions = 0;

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
        BisimulationBasedHeuristicSearchEngine* res =
            new BisimulationBasedHeuristicSearchEngine(
                engine_name,
                state_registry);

        using HeuristicSearchType =
            HS<bisimulation::QuotientState,
               bisimulation::QuotientAction,
               DualValues>;

        res->engine_ = std::unique_ptr<engines::MDPEngine<
            bisimulation::QuotientState,
            bisimulation::QuotientAction>>(
            new HeuristicSearchType(
                &res->state_id_map,
                &res->action_id_map,
                res->state_reward.get(),
                &res->transition_reward,
                g_analysis_objective->min(),
                g_analysis_objective->max(),
                res->aops_gen.get(),
                res->tgen.get(),
                DeadEndIdentificationLevel::Disabled,
                nullptr,
                &res->dead_end_listener_,
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

    template <
        template <typename, typename, typename>
        class HS,
        typename DualValues,
        typename... Args>
    static BisimulationBasedHeuristicSearchEngine* QConstructor(
        const DualValues&,
        const std::string& engine_name,
        StateRegistry* state_registry,
        HeuristicSearchConnector& con,
        ProgressReport& progress,
        bool interval,
        bool stable_policy,
        Args... args)
    {
        BisimulationBasedHeuristicSearchEngine* res =
            new BisimulationBasedHeuristicSearchEngine(
                engine_name,
                state_registry,
                true);

        res->engine_ = std::unique_ptr<
            MDPEngineInterface<bisimulation::QuotientState>>(
            new HS<
                bisimulation::QuotientState,
                quotient_system::QuotientAction<bisimulation::QuotientAction>,
                DualValues>(
                args...,
                res->quotient_.get(),
                DeadEndIdentificationLevel::Disabled,
                nullptr,
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
                res->state_reward.get(),
                res->q_action_reward_.get(),
                g_analysis_objective->min(),
                g_analysis_objective->max(),
                res->q_aops_gen_.get(),
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
        BisimulationBasedHeuristicSearchEngine* res =
            new BisimulationBasedHeuristicSearchEngine(
                engine_name,
                state_registry,
                true);

        using FretVariant = Fret<
            bisimulation::QuotientState,
            bisimulation::QuotientAction,
            DualValues>;

        engines::fret::HeuristicSearchEngine<
            bisimulation::QuotientState,
            quotient_system::QuotientAction<bisimulation::QuotientAction>,
            DualValues>* engine =
            new HS<
                bisimulation::QuotientState,
                quotient_system::QuotientAction<bisimulation::QuotientAction>,
                DualValues>(
                &res->state_id_map,
                res->q_action_id_map_.get(),
                res->state_reward.get(),
                res->q_action_reward_.get(),
                g_analysis_objective->min(),
                g_analysis_objective->max(),
                res->q_aops_gen_.get(),
                res->q_transition_gen_.get(),
                DeadEndIdentificationLevel::Disabled,
                nullptr,
                nullptr,
                res->q_policy_tiebreaker_.get(),
                &res->new_state_handler_,
                res->heuristic_.get(),
                &con,
                &progress,
                interval,
                stable_policy,
                args...);

        res->engine2_ = std::unique_ptr<
            engines::MDPEngineInterface<bisimulation::QuotientState>>(engine);

        res->engine_ =
            std::unique_ptr<MDPEngineInterface<bisimulation::QuotientState>>(
                new FretVariant(
                    &res->state_id_map,
                    &res->action_id_map,
                    res->state_reward.get(),
                    &res->transition_reward,
                    g_analysis_objective->min(),
                    g_analysis_objective->max(),
                    res->aops_gen.get(),
                    res->tgen.get(),
                    res->quotient_.get(),
                    &progress,
                    engine));

        return res;
    }

    virtual void solve(const GlobalState&) override
    {
        logging::out << "Running " << engine_name_ << "..." << std::endl;
        engine_->solve(bs->get_initial_state());
        stats.extended_states = bs->num_extended_states();
    }

    virtual value_type::value_t get_result(const GlobalState&) override
    {
        return engine_->get_result(bs->get_initial_state());
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

private:
    explicit BisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name,
        StateRegistry* state_registry)
        : engine_name_(engine_name)
    {
        logging::out << "Building bisimulation..." << std::endl;
        bs = std::unique_ptr<bisimulation::BisimilarStateSpace>(
            new bisimulation::BisimilarStateSpace(
                state_registry->get_initial_state(),
                g_step_bound,
                g_step_cost_type));
        aops_gen = std::unique_ptr<
            ApplicableActionsGenerator<bisimulation::QuotientAction>>(
            new ApplicableActionsGenerator<bisimulation::QuotientAction>(
                bs.get()));
        tgen =
            std::unique_ptr<TransitionGenerator<bisimulation::QuotientAction>>(
                new TransitionGenerator<bisimulation::QuotientAction>(
                    bs.get()));
        state_reward =
            std::unique_ptr<bisimulation::QuotientStateRewardFunction>(
                new bisimulation::DefaultQuotientStateRewardFunction(
                    bs.get(),
                    g_analysis_objective->min(),
                    g_analysis_objective->max()));
        heuristic_ =
            std::unique_ptr<StateEvaluator<bisimulation::QuotientState>>(
                new bisimulation::DefaultQuotientStateEvaluator(
                    bs.get(),
                    g_analysis_objective->min(),
                    g_analysis_objective->max(),
                    g_analysis_objective->max()));

        stats.timer.stop();
        stats.states = bs->num_bisimilar_states();
        stats.transitions = bs->num_transitions();

        logging::out << "Bisimulation built after " << stats.timer << std::endl;
        logging::out << "Bisimilar state space contains "
                     << bs->num_bisimilar_states() << " states and "
                     << bs->num_transitions() << " transitions." << std::endl;
        logging::out << std::endl;
    }

    explicit BisimulationBasedHeuristicSearchEngine(
        const std::string& engine_name,
        StateRegistry* state_registry,
        bool)
        : BisimulationBasedHeuristicSearchEngine(engine_name, state_registry)
    {
        quotient_ = std::unique_ptr<
            quotient_system::QuotientSystem<bisimulation::QuotientAction>>(
            new quotient_system::QuotientSystem<bisimulation::QuotientAction>(
                &action_id_map,
                aops_gen.get(),
                tgen.get()));
        q_action_reward_ = std::unique_ptr<ActionRewardFunction<
            quotient_system::QuotientAction<bisimulation::QuotientAction>>>(
            new quotient_system::DefaultQuotientActionRewardFunction(
                quotient_.get(),
                &transition_reward));
        q_action_id_map_ = std::unique_ptr<ActionIDMap<
            quotient_system::QuotientAction<bisimulation::QuotientAction>>>(
            new ActionIDMap<
                quotient_system::QuotientAction<bisimulation::QuotientAction>>(
                quotient_.get()));
        q_aops_gen_ = std::unique_ptr<ApplicableActionsGenerator<
            quotient_system::QuotientAction<bisimulation::QuotientAction>>>(
            new ApplicableActionsGenerator<
                quotient_system::QuotientAction<bisimulation::QuotientAction>>(
                quotient_.get()));
        q_transition_gen_ = std::unique_ptr<TransitionGenerator<
            quotient_system::QuotientAction<bisimulation::QuotientAction>>>(
            new TransitionGenerator<
                quotient_system::QuotientAction<bisimulation::QuotientAction>>(
                quotient_.get()));

        q_policy_tiebreaker_ = std::unique_ptr<PolicyPicker<
            quotient_system::QuotientAction<bisimulation::QuotientAction>>>(
            new PolicyPicker<quotient_system::QuotientAction<
                bisimulation::QuotientAction>>());
    }

    const std::string engine_name_;

    BisimulationTimer stats;

    std::unique_ptr<bisimulation::BisimilarStateSpace> bs = nullptr;

    StateIDMap<bisimulation::QuotientState> state_id_map;
    ActionIDMap<bisimulation::QuotientAction> action_id_map;
    std::unique_ptr<ApplicableActionsGenerator<bisimulation::QuotientAction>>
        aops_gen = nullptr;
    std::unique_ptr<TransitionGenerator<bisimulation::QuotientAction>> tgen =
        nullptr;
    std::unique_ptr<StateRewardFunction<bisimulation::QuotientState>>
        state_reward = nullptr;
    bisimulation::DefaultQuotientActionRewardFunction transition_reward;
    DeadEndListener<bisimulation::QuotientState, bisimulation::QuotientAction>
        dead_end_listener_;
    NewStateHandler<bisimulation::QuotientState> new_state_handler_;
    std::unique_ptr<StateEvaluator<bisimulation::QuotientState>> heuristic_ =
        nullptr;
    PolicyPicker<bisimulation::QuotientAction> policy_;

    std::unique_ptr<MDPEngineInterface<bisimulation::QuotientState>> engine_ =
        nullptr;

    std::unique_ptr<
        quotient_system::QuotientSystem<bisimulation::QuotientAction>>
        quotient_ = nullptr;

    std::unique_ptr<ActionRewardFunction<
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_action_reward_ = nullptr;
    std::unique_ptr<ActionIDMap<
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_action_id_map_ = nullptr;
    std::unique_ptr<ApplicableActionsGenerator<
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_aops_gen_ = nullptr;
    std::unique_ptr<TransitionGenerator<
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_transition_gen_ = nullptr;

    std::unique_ptr<PolicyPicker<
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_policy_tiebreaker_ = nullptr;
    std::unique_ptr<DeadEndListener<
        GlobalState,
        quotient_system::QuotientAction<bisimulation::QuotientAction>>>
        q_dead_end_listener_ = nullptr;

    std::unique_ptr<MDPEngineInterface<bisimulation::QuotientState>> engine2_ =
        nullptr;
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
            return BisimulationBasedHeuristicSearchEngine::
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
            return BisimulationBasedHeuristicSearchEngine::
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
        return BisimulationBasedHeuristicSearchEngine::
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
} // namespace probabilistic
