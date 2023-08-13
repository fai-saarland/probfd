#ifndef PROBFD_TASK_STATE_SPACE_H
#define PROBFD_TASK_STATE_SPACE_H

#include "probfd/storage/per_state_storage.h"
#include "probfd/storage/segmented_memory_pool.h"

#include "probfd/mdp.h"
#include "probfd/task_proxy.h"
#include "probfd/task_types.h"

#include "downward/algorithms/segmented_vector.h"

#include "downward/task_utils/successor_generator.h"

#include "downward/state_registry.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class State;
class StateRegistry;
class Evaluator;

namespace probfd {
class ProbabilisticTask;

class InducedTaskStateSpace : public TaskSimpleMDP {
protected:
    struct Statistics {
        unsigned long long single_transition_generator_calls = 0;
        unsigned long long all_transitions_generator_calls = 0;
        unsigned long long aops_generator_calls = 0;

        unsigned long long generated_operators = 0;
        unsigned long long generated_states = 0;

        unsigned long long aops_computations = 0;
        unsigned long long computed_operators = 0;

        unsigned long long transition_computations = 0;
        unsigned long long computed_successors = 0;

        void print(utils::LogProxy log) const;
    };

protected:
    ProbabilisticTaskProxy task_proxy;
    mutable utils::LogProxy log;

    successor_generator::SuccessorGenerator gen_;
    StateRegistry state_registry_;

    const std::shared_ptr<TaskSimpleCostFunction> cost_function_;

    const std::vector<std::shared_ptr<::Evaluator>> notify_;

    std::vector<OperatorID> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;

public:
    InducedTaskStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        std::shared_ptr<TaskSimpleCostFunction> cost_function,
        const std::vector<std::shared_ptr<::Evaluator>>&
            path_dependent_evaluators = {});

    StateID get_state_id(const State& state) override final;
    State get_state(StateID state_id) override final;

    void generate_applicable_actions(
        StateID state_id,
        std::vector<OperatorID>& result) override;

    void generate_action_transitions(
        StateID state,
        OperatorID operator_id,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void generate_all_transitions(
        StateID state,
        std::vector<Transition>& transitions) override;

    value_t get_action_cost(OperatorID op) override;

    bool is_goal(const State& state) const override final;
    value_t get_non_goal_termination_cost() const override final;

    const State& get_initial_state();

    size_t get_num_registered_states() const;

    void print_statistics() const override;

    void compute_successor_dist(
        const State& s,
        OperatorID op_id,
        Distribution<StateID>& successors);

protected:
    void
    compute_applicable_operators(const State& s, std::vector<OperatorID>& ops);
};

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__