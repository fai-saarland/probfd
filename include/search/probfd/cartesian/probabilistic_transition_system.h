#ifndef PROBFD_CARTESIAN_PROBABILISTIC_TRANSITION_SYSTEM_H
#define PROBFD_CARTESIAN_PROBABILISTIC_TRANSITION_SYSTEM_H

#include "probfd/cartesian/probabilistic_transition.h"
#include "probfd/cartesian/types.h"

#include "probfd/task_proxy.h"

#include <deque>
#include <set>
#include <vector>

struct FactPair;
class ProbabilisticOperatorsProxy;

namespace utils {
class LogProxy;
}

namespace probfd {
namespace cartesian {

class ProbabilisticTransitionSystem {
    // Operator information
    const std::vector<std::vector<FactPair>> preconditions_by_operator;
    const std::vector<std::vector<std::vector<FactPair>>>
        postconditions_by_operator_and_outcome;
    const std::vector<std::vector<value_t>>
        probabilities_by_operator_and_outcome;

    // References to incoming and outgoing transitions
    std::vector<std::vector<ProbabilisticTransition*>> outgoing;
    std::vector<std::vector<ProbabilisticTransition*>> incoming;

    // The transition list. Using deque here to avoid invalidating references.
    std::deque<ProbabilisticTransition> transitions;

    // The list of uniform self-loops, to be pruned during search.
    std::deque<std::vector<int>> loops;

    size_t num_loops = 0;

    // Increases size of incoming and outgoing transition lists by one.
    void enlarge_vectors_by_one();

    // Construct the trivial abstraction.
    void construct_trivial_abstraction(const ProbabilisticOperatorsProxy& ops);

    int get_precondition_value(int op_id, int var) const;
    int get_postcondition_value(int op_id, int eff_id, int var) const;
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

    // Update transition system after v has been split for var into v1 and v2.
    void rewire(
        const AbstractStates& states,
        const AbstractState& v1,
        const AbstractState& v2,
        int var);

    value_t get_probability(int op_index, int eff_index) const;

    const std::vector<std::vector<ProbabilisticTransition*>>&
    get_incoming_transitions() const;
    const std::vector<std::vector<ProbabilisticTransition*>>&
    get_outgoing_transitions() const;
    const std::deque<std::vector<int>>& get_loops() const;

    const std::deque<ProbabilisticTransition>& get_transitions() const;

    int get_num_states() const;
    int get_num_operators() const;
    int get_num_non_loops() const;
    int get_num_loops() const;

    void print_statistics(utils::LogProxy& log) const;
};

} // namespace cartesian
} // namespace probfd

#endif // PROBFD_CARTESIAN_PROBABILISTIC_TRANSITION_SYSTEM_H
