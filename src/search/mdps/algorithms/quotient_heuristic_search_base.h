#include "heuristic_search_base.h"
#include "quotient_system.h"

namespace probabilistic {
namespace algorithms {
namespace heuristic_search_base {
template<
    typename StateT,
    typename ActionT,
    typename DualBounds,
    typename StorePolicy,
    template<typename>
    class StateInfoWrapper>
class QHeuristicSearchBase
    : public HeuristicSearchBase<
          quotient_system::QuotientState<StateT, ActionT>,
          quotient_system::QuotientAction<StateT, ActionT>,
          DualBounds,
          StorePolicy,
          StateInfoWrapper>,
      public IMDPEngine<StateT> {
public:
    using State = StateT;
    using Action = ActionT;
    using QuotientSystem = quotient_system::QuotientSystem<StateT, ActionT>;
    using QuotientSystemFunctionRegistry =
        quotient_system::QuotientSystemFunctionRegistry<StateT, ActionT>;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;

    QHeuristicSearchBase(
        ProgressReport* report,
        StateIDMap<State>* state_id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        StateEvaluationFunction<State>* value_init,
        PolicyChooser<QState, QAction>* policy_chooser,
        value_type::value_t dead_end_value,
        NewStateHandler<State, Action>* new_state_handler,
        bool interval_comparison = false)
        : QHeuristicSearchBase(
            new QuotientSystemFunctionRegistry(new QuotientSystem(
                false,
                state_id_map,
                aops_gen,
                transition_gen)),
            report,
            state_reward,
            transition_reward,
            value_init,
            policy_chooser,
            dead_end_value,
            new_state_handler,
            interval_comparison)
    {
    }

    virtual ~QHeuristicSearchBase()
    {
        if (fn_registry_ != nullptr) {
            delete (fn_registry_);
        }
        if (quotient_ != nullptr) {
            delete (quotient_);
        }
    }

    virtual AnalysisResult solve(const QState& qstate) override
    {
        return this->qsolve(qstate);
    }

    virtual AnalysisResult solve(const State& state) override
    {
        const QState qstate = quotient_->get_quotient_state(state);
        return this->qsolve(qstate);
    }

protected:
    virtual AnalysisResult qsolve(const QState& qstate) = 0;

    QuotientSystem* quotient_;
    QuotientSystemFunctionRegistry* fn_registry_;

private:
    QHeuristicSearchBase(
        QuotientSystemFunctionRegistry* r,
        ProgressReport* report,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        StateEvaluationFunction<State>* value_init,
        PolicyChooser<QState, QAction>* policy_chooser,
        value_type::value_t dead_end_value,
        NewStateHandler<State, Action>* new_state_handler,
        bool interval_comparison = false)
        : HeuristicSearchBase<
            QState,
            QAction,
            DualBounds,
            StorePolicy,
            StateInfoWrapper>(
            report,
            r->get_id_map(),
            r->get_aops_gen(),
            r->get_transition_gen(),
            r->create(state_reward),
            r->create(transition_reward),
            r->create(value_init),
            policy_chooser,
            dead_end_value,
            r->create(new_state_handler),
            interval_comparison)
        , quotient_(r->quotient())
        , fn_registry_(r)
    {
    }
};

} // namespace heuristic_search_base
} // namespace algorithms
} // namespace probabilistic

