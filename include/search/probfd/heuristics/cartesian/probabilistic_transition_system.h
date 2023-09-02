#ifndef PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_SYSTEM_H
#define PROBFD_HEURISTICS_CARTESIAN_PROBABILISTIC_TRANSITION_SYSTEM_H

#include "probfd/heuristics/cartesian/probabilistic_transition.h"
#include "probfd/heuristics/cartesian/types.h"

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
namespace heuristics {
namespace cartesian {

class ProbabilisticTransitionSystem {
    // Operator information
    const std::vector<std::vector<FactPair>> preconditions_by_operator;
    const std::vector<std::vector<std::vector<FactPair>>>
        postconditions_by_operator_and_outcome;
    const std::vector<std::vector<value_t>>
        probabilities_by_operator_and_outcome;

    struct TransitionProxy {
        ProbabilisticTransition* transition;
        int source_id;
    };

    // The outgoing transitions for every abstract state. Using deque here to
    // avoid invalidating references stored in the proxy nodes.
    // Note: Cannot use vector of deque because deque's move constructor is not
    // noexcept, which means vector copies when it grows. That's not only slow,
    // it also invalidates the references to the transitions.
    std::deque<std::deque<ProbabilisticTransition>> outgoing_by_id;

    // List of proxy nodes pointing to the transitions. Using deque here to
    // avoid invalidating references to the proxy nodes (incoming_by_id and
    // proxies_by_id).
    std::deque<TransitionProxy> proxy_nodes;

    // References to the proxy nodes for every abstract state.
    std::vector<std::vector<TransitionProxy*>> proxies_by_id;

    // References to incoming transitions for every abstract state.
    std::vector<std::vector<TransitionProxy*>> incoming_by_id;

    // The list of uniform self-loops, to be pruned during search.
    std::vector<std::vector<int>> loops_by_id;

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

    const std::vector<std::vector<TransitionProxy*>>&
    get_incoming_transitions() const;
    const std::deque<std::deque<ProbabilisticTransition>>&
    get_outgoing_transitions() const;
    const std::vector<std::vector<int>>& get_loops() const;

    int get_num_states() const;
    int get_num_operators() const;
    int get_num_non_loops() const;
    int get_num_loops() const;

    void print_statistics(utils::LogProxy& log) const;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
