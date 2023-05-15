#ifndef PROBFD_TASK_STATE_SPACE_H
#define PROBFD_TASK_STATE_SPACE_H

#include "probfd/engine_interfaces/state_space.h"

#include "probfd/storage/per_state_storage.h"
#include "probfd/storage/segmented_memory_pool.h"

#include "probfd/task_proxy.h"

#include "algorithms/segmented_vector.h"

#include "task_utils/successor_generator.h"

#include "state_registry.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class State;
class StateRegistry;
class Evaluator;

namespace probfd {
class ProbabilisticTask;

class TaskStateSpace : public engine_interfaces::StateSpace<State, OperatorID> {
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

        void print(std::ostream& out) const;
    };

protected:
    ProbabilisticTaskProxy task_proxy;

    successor_generator::SuccessorGenerator gen_;
    StateRegistry state_registry_;

    const std::vector<std::shared_ptr<::Evaluator>> notify_;

    std::vector<OperatorID> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;

public:
    TaskStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        const std::vector<std::shared_ptr<::Evaluator>>&
            path_dependent_evaluators = {});

    StateID get_state_id(const State& state) override;
    State get_state(StateID state_id) override;

    ActionID get_action_id(StateID, OperatorID op_id) override;
    OperatorID get_action(StateID, ActionID action_id) override;

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

    const State& get_initial_state();

    size_t get_num_registered_states() const;

    virtual void print_statistics(std::ostream& out) const;

protected:
    void
    compute_applicable_operators(const State& s, std::vector<OperatorID>& ops);

    void compute_successor_states(
        const State& s,
        OperatorID op_id,
        std::vector<StateID>& successors);

    size_t compute_successor_dist(
        const State& s,
        OperatorID op_id,
        Distribution<StateID>& successors);
};

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__