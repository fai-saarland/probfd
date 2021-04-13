#pragma once

#include "../../global_state.h"
#include "../../solver_interface.h"
#include "../../state_registry.h"
#include "../action_evaluator.h"
#include "../action_id_map.h"
#include "../engines/engine.h"
#include "../probabilistic_operator.h"
#include "../progress_report.h"
#include "../state_evaluator.h"
#include "../state_id_map.h"
#include "../transition_generator.h"

#include <memory>
#include <string>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

class MDPSolver : public SolverInterface {
public:
    explicit MDPSolver(const options::Options& opts);

    template<typename Engine, typename... Args>
    engines::MDPEngine<GlobalState, const ProbabilisticOperator*>*
    engine_factory(Args... args)
    {
        return new Engine(
            args...,
            &state_id_map_,
            &action_id_map_,
            state_reward_function_.get(),
            action_reward_function_.get(),
            minimal_reward_,
            maximal_reward_,
            &aops_generator_,
            &transition_generator_);
    }

    virtual engines::MDPEngineInterface<GlobalState>* create_engine() = 0;
    virtual std::string get_engine_name() const = 0;
    virtual void print_additional_statistics() const { }

    virtual void solve() override;
    virtual bool found_solution() const override { return true; }

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    StateIDMap<GlobalState>* get_state_id_map() { return &state_id_map_; }

    ActionIDMap<const ProbabilisticOperator*>* get_action_id_map()
    {
        return &action_id_map_;
    }

    ActionEvaluator<const ProbabilisticOperator*>* get_action_reward_function()
    {
        return action_reward_function_.get();
    }

    StateEvaluator<GlobalState>* get_state_reward_function()
    {
        return state_reward_function_.get();
    }

    ApplicableActionsGenerator<const ProbabilisticOperator*>*
    get_applicable_actions_generator()
    {
        return &aops_generator_;
    }

    TransitionGenerator<const ProbabilisticOperator*>*
    get_transition_generator()
    {
        return &transition_generator_;
    }

    value_type::value_t get_minimal_reward() const { return minimal_reward_; }

    value_type::value_t get_maximal_reward() const { return maximal_reward_; }

    StateRegistry* get_state_registry() { return &state_registry_; }

    ProgressReport progress_;

private:
    StateRegistry state_registry_;

    StateIDMap<GlobalState> state_id_map_;
    ActionIDMap<const ProbabilisticOperator*> action_id_map_;
    std::shared_ptr<StateEvaluator<GlobalState>> state_reward_function_;
    std::shared_ptr<ActionEvaluator<const ProbabilisticOperator*>>
        action_reward_function_;
    value_type::value_t minimal_reward_;
    value_type::value_t maximal_reward_;
    TransitionGenerator<const ProbabilisticOperator*> transition_generator_;
    ApplicableActionsGenerator<const ProbabilisticOperator*> aops_generator_;
};

} // namespace probabilistic
