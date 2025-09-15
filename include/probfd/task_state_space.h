#ifndef PROBFD_TASK_STATE_SPACE_H
#define PROBFD_TASK_STATE_SPACE_H

#include "cartesian_abstractions/probabilistic_transition_system.h"
#include "probabilistic_task.h"
#include "probfd/task_utils/probabilistic_successor_generator.h"

#include "probfd/fdr_types.h"
#include "probfd/mdp.h"
#include "probfd/state_id.h"
#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include "downward/state.h"
#include "downward/state_registry.h"

#include <cstddef>
#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class OperatorID;
class Evaluator;
} // namespace downward

namespace probfd {
template <typename>
class Distribution;
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

    std::shared_ptr<ProbabilisticOperatorSpace> operators_;
    downward::StateRegistry state_registry_;
    successor_generator::ProbabilisticSuccessorGenerator gen_;

    const std::vector<std::shared_ptr<downward::Evaluator>> notify_;

    Statistics statistics_;

public:
    explicit TaskStateSpace(
        const downward::VariableSpace& variables,
        const downward::AxiomSpace& axioms,
        std::shared_ptr<ProbabilisticOperatorSpace> operators,
        const downward::InitialStateValues& initial_values,
        std::vector<std::shared_ptr<downward::Evaluator>>
            path_dependent_evaluators = {});

    StateID get_state_id(const downward::State& state) final;
    downward::State get_state(StateID state_id) final;

    void generate_applicable_actions(
        const downward::State& state,
        std::vector<downward::OperatorID>& result) override;

    void generate_action_transitions(
        const downward::State& state,
        downward::OperatorID operator_id,
        SuccessorDistribution& successor_dist) override;

    void generate_all_transitions(
        const downward::State& state,
        std::vector<downward::OperatorID>& aops,
        std::vector<SuccessorDistribution>& successor_dist) override;

    void generate_all_transitions(
        const downward::State& state,
        std::vector<TransitionTailType>& transitions) override;

    const downward::State& get_initial_state();

    size_t get_num_registered_states() const;

    virtual void print_statistics(std::ostream& out) const;

    void compute_successor_dist(
        const downward::State& s,
        downward::OperatorID op_id,
        SuccessorDistribution& successor_dist);

protected:
    void compute_applicable_operators(
        const downward::State& s,
        std::vector<downward::OperatorID>& ops);
};

} // namespace probfd

#endif // PROBFD_TASK_STATE_SPACE_H