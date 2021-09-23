#ifndef MDPS_SOLVERS_MDP_SOLVER_H
#define MDPS_SOLVERS_MDP_SOLVER_H

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

/// Namespace dedicated to MDP solver interfaces.
namespace solvers {

/**
 * @brief Base interface for MDP solvers.
 */
class MDPSolver : public SolverInterface {
public:
    /**
     * @brief Constructs the MDP solver from the given options.
     */
    explicit MDPSolver(const options::Options& opts);

    /**
     * @brief Factory method that constructs an new MDP engine from the given
     * arguments.
     *
     * @tparam Engine - The engine type to construct.
     */
    template <typename Engine, typename... Args>
    engines::MDPEngine<GlobalState, const ProbabilisticOperator*>*
    engine_factory(Args&&... args)
    {
        return new Engine(
            &state_id_map_,
            &action_id_map_,
            state_reward_function_,
            action_reward_function_,
            minimal_reward_,
            maximal_reward_,
            &aops_generator_,
            &transition_generator_,
            std::forward<Args>(args)...);
    }

    /**
     * @brief Factory method a new instance of the encapsulated MDP engine.
     */
    virtual engines::MDPEngineInterface<GlobalState>* create_engine() = 0;

    /**
     * @brief Returns the name of the encapsulated MDP engine.
     */
    virtual std::string get_engine_name() const = 0;

    /**
     * @brief Print additional engine statistics to std::cout.
     */
    virtual void print_additional_statistics() const {}

    /**
     * @brief Runs the encapsulated MDP on the global problem.
     */
    virtual void solve() override;

    /**
     * @brief Checks if the MDP engine found a solution.
     */
    virtual bool found_solution() const override { return true; }

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    StateIDMap<GlobalState>* get_state_id_map() { return &state_id_map_; }

    ActionIDMap<const ProbabilisticOperator*>* get_action_id_map()
    {
        return &action_id_map_;
    }

    ActionRewardFunction<const ProbabilisticOperator*>*
    get_action_reward_function()
    {
        return action_reward_function_;
    }

    StateRewardFunction<GlobalState>* get_state_reward_function()
    {
        return state_reward_function_;
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
    StateRewardFunction<GlobalState>* state_reward_function_;
    ActionRewardFunction<const ProbabilisticOperator*>* action_reward_function_;
    value_type::value_t minimal_reward_;
    value_type::value_t maximal_reward_;
    TransitionGenerator<const ProbabilisticOperator*> transition_generator_;
    ApplicableActionsGenerator<const ProbabilisticOperator*> aops_generator_;
};

} // namespace solvers
} // namespace probabilistic

#endif // __MDP_SOLVER_H__