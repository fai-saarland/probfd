#pragma once

#include "../../global_state.h"
#include "../../operator_cost.h"
#include "../../solver_interface.h"
#include "../../state_id.h"
#include "../../state_registry.h"
#include "../algorithms/quotient_system.h"
#include "../algorithms/types_common.h"
#include "../algorithms/types_heuristic_search.h"
#include "../analysis_objective.h"
#include "../analysis_result.h"
#include "../applicable_actions_generator.h"
#include "../interfaces/i_engine.h"
#include "../new_state_handler.h"
#include "../progress_report.h"
#include "../transition_generator.h"

#include <memory>
#include <string>
#include <vector>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace algorithms {
template<>
class StateIDMap<GlobalState> {
public:
    explicit StateIDMap(StateRegistry* registry)
        : registry_(registry)
    {
    }

    StateID operator[](const GlobalState& state) const
    {
        return state.get_id();
    }

    GlobalState operator[](const StateID& state_id) const
    {
        return registry_->lookup_state(state_id);
    }

private:
    StateRegistry* registry_;
};

template<>
struct NullAction<const ProbabilisticOperator*> {
    const ProbabilisticOperator* operator()() const { return nullptr; }
};

template<>
struct NullAction<algorithms::quotient_system::QuotientAction<
    GlobalState,
    const ProbabilisticOperator*>> {
    const algorithms::quotient_system::
        QuotientAction<GlobalState, const ProbabilisticOperator*>
        operator()() const
    {
        return algorithms::quotient_system::
            QuotientAction<GlobalState, const ProbabilisticOperator*>(
                GlobalState(nullptr, nullptr), nullptr);
    }
};

} // namespace algorithms

namespace solvers {
using State = GlobalState;
using Action = const ProbabilisticOperator*;
using QuotientSystem =
    algorithms::quotient_system::QuotientSystem<State, Action>;
using QState = QuotientSystem::QState;
using QAction = QuotientSystem::QAction;
using QuotientSystemFunctionFactory =
    algorithms::quotient_system::QuotientSystemFunctionRegistry<State, Action>;

class MDPSolver : public SolverInterface {
public:
    MDPSolver(const options::Options& opts);
    virtual bool found_solution() const override { return true; }
    virtual void solve() override;

    virtual std::string name() const = 0;

    virtual IMDPEngine<GlobalState>* create_engine(
        ProgressReport* report,
        algorithms::StateIDMap<GlobalState>* state_id_map,
        algorithms::ApplicableActionsGenerator<
            GlobalState,
            const ProbabilisticOperator*>* aops_generator,
        algorithms::TransitionGenerator<
            GlobalState,
            const ProbabilisticOperator*>* transition_generator,
        value_type::value_t minval,
        value_type::value_t maxval,
        algorithms::StateEvaluationFunction<GlobalState>* state_reward,
        algorithms::TransitionRewardFunction<
            GlobalState,
            const ProbabilisticOperator*>* transition_reward) = 0;

    virtual IMDPEngine<QuotientSystem::QState>* create_engine(
        QuotientSystemFunctionFactory& factory,
        QuotientSystem* /*quotient*/,
        ProgressReport* /*report*/,
        algorithms::StateIDMap<QuotientSystem::QState>* /*state_id_map*/,
        algorithms::ApplicableActionsGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* /*aops_generator*/,
        algorithms::TransitionGenerator<
            QuotientSystem::QState,
            QuotientSystem::QAction>* /*transition_generator*/,
        value_type::value_t /*minval*/,
        value_type::value_t /*maxval*/,
        algorithms::StateEvaluationFunction<
            QuotientSystem::QState>* /*state_reward*/,
        algorithms::TransitionRewardFunction<
            QuotientSystem::QState,
            QuotientSystem::QAction>* /*transition_reward*/);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual bool maintains_dual_bounds() const { return false; }

    virtual void print_additional_statistics(std::ostream&) const { }

protected:

    std::shared_ptr<NewGlobalStateHandler> handler_;

private:
    const bool cache_aops_;
    const bool cache_transitions_;
    const std::vector<std::shared_ptr<Heuristic>> path_dependent_heuristics_;
    const value_type::value_t report_difference_;
};

} // namespace solvers
} // namespace probabilistic
