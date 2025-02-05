#ifndef PROBFD_TASK_STATE_SPACE_H
#define PROBFD_TASK_STATE_SPACE_H

#include "probfd/task_utils/probabilistic_successor_generator.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"
#include "probfd/state_id.h"
#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include "downward/state_registry.h"
#include "downward/task_proxy.h"

#include <cstddef>
#include <memory>
#include <vector>

// Forward Declarations
class OperatorID;
class Evaluator;

namespace probfd {
template <typename>
class Distribution;
class ProbabilisticTask;
} // namespace probfd

namespace probfd {

class TaskStateSpace : public FDRStateSpace {
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
    ProbabilisticTaskProxy task_proxy_;
    StateRegistry state_registry_;
    successor_generator::ProbabilisticSuccessorGenerator gen_;

    const std::vector<std::shared_ptr<::Evaluator>> notify_;

    Statistics statistics_;

public:
    TaskStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators =
            {});

    StateID get_state_id(const State& state) final;
    State get_state(StateID state_id) final;

    void generate_applicable_actions(
        const State& state,
        std::vector<OperatorID>& result) override;

    void generate_action_transitions(
        const State& state,
        OperatorID operator_id,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        const State& state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void generate_all_transitions(
        const State& state,
        std::vector<TransitionType>& transitions) override;

    const State& get_initial_state();

    size_t get_num_registered_states() const;

    virtual void print_statistics(std::ostream& out) const;

    void compute_successor_dist(
        const State& s,
        OperatorID op_id,
        Distribution<StateID>& successors);

protected:
    void
    compute_applicable_operators(const State& s, std::vector<OperatorID>& ops);
};

} // namespace probfd

#endif // PROBFD_TASK_STATE_SPACE_H