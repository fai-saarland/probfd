#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_SYSTEM_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_SYSTEM_H

#include "probfd/cartesian_abstractions/probabilistic_transition.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/value_type.h"

#include <cstddef>
#include <deque>
#include <vector>

// Forward Declarations
namespace downward {
struct FactPair;
}

namespace downward::utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticOperatorsProxy;
}

namespace probfd::cartesian_abstractions {
class AbstractState;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

class ProbabilisticTransitionSystem {
    // Operator information
    const std::vector<std::vector<downward::FactPair>>
        preconditions_by_operator_;
    const std::vector<std::vector<std::vector<downward::FactPair>>>
        postconditions_by_operator_and_outcome_;
    const std::vector<std::vector<value_t>>
        probabilities_by_operator_and_outcome_;

    // References to incoming and outgoing transitions
    std::vector<std::vector<ProbabilisticTransition*>> outgoing_;
    std::vector<std::vector<ProbabilisticTransition*>> incoming_;

    // The transition list. Using deque here to avoid invalidating references.
    std::deque<ProbabilisticTransition> transitions_;

    // The list of uniform self-loops, to be pruned during search.
    std::deque<std::vector<int>> loops_;

    size_t num_loops_ = 0;

    // Increases size of incoming and outgoing transition lists by one.
    void enlarge_vectors_by_one();

    // Construct the trivial abstraction.
    void construct_trivial_abstraction(const ProbabilisticOperatorsProxy& ops);

    [[nodiscard]]
    int get_precondition_value(int op_id, int var) const;

    [[nodiscard]]
    int get_postcondition_value(int op_id, std::size_t eff_id, int var) const;

    [[nodiscard]]
    size_t get_num_operator_outcomes(int op_id) const;

    void add_transition(int src_id, int op_id, std::vector<int> target_ids);
    void add_loop(int src_id, int op_id);

    void rewire_incoming_transitions(
        const AbstractStates& states,
        const AbstractState& v1,
        const AbstractState& v2,
        int var);
    void rewire_outgoing_transitions(
        const AbstractStates& states,
        const AbstractState& v1,
        const AbstractState& v2,
        int var);

    void
    rewire_loops(const AbstractState& v1, const AbstractState& v2, int var);

public:
    explicit ProbabilisticTransitionSystem(
        const ProbabilisticOperatorsProxy& ops);

    ~ProbabilisticTransitionSystem();

    // Update transition system after v has been split for var into v1 and v2.
    void rewire(
        const AbstractStates& states,
        const AbstractState& v1,
        const AbstractState& v2,
        int var);

    [[nodiscard]]
    value_t get_probability(int op_index, int eff_index) const;

    [[nodiscard]]
    const std::vector<std::vector<ProbabilisticTransition*>>&
    get_incoming_transitions() const;

    [[nodiscard]]
    const std::vector<std::vector<ProbabilisticTransition*>>&
    get_outgoing_transitions() const;

    [[nodiscard]]
    const std::deque<std::vector<int>>& get_loops() const;

    [[nodiscard]]
    const std::deque<ProbabilisticTransition>& get_transitions() const;

    [[nodiscard]]
    int get_num_states() const;

    [[nodiscard]]
    int get_num_operators() const;

    [[nodiscard]]
    int get_num_non_loops() const;

    [[nodiscard]]
    int get_num_loops() const;

    void print_statistics(downward::utils::LogProxy& log) const;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_PROBABILISTIC_TRANSITION_SYSTEM_H
