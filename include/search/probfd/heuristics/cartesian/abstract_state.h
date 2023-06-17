#ifndef PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_STATE_H
#define PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_STATE_H

#include "downward/cegar/cartesian_set.h"
#include "probfd/heuristics/cartesian/types.h"

#include <vector>

class ConditionsProxy;
struct FactPair;
class State;
class TaskProxy;

namespace probfd {
class ProbabilisticOperatorProxy;
class ProbabilisticEffectsProxy;

namespace heuristics {
namespace cartesian {

class Node;

/*
  Store the Cartesian set and the ID of the node in the refinement hierarchy
  for an abstract state.
*/
class AbstractState {
    int state_id;

    // This state's node in the refinement hierarchy.
    NodeID node_id;

    CartesianSet cartesian_set;

public:
    AbstractState(int state_id, NodeID node_id, CartesianSet&& cartesian_set);

    AbstractState(const AbstractState&) = delete;

    bool domain_subsets_intersect(const AbstractState& other, int var) const;

    // Return the size of var's abstract domain for this state.
    int count(int var) const;

    bool contains(int var, int value) const;

    // Return the Cartesian set in which applying "effect" of the operator "op"
    // can lead to this state.
    CartesianSet regress(
        const ProbabilisticOperatorProxy& op,
        const ProbabilisticEffectsProxy& effects) const;

    /*
      Separate the "wanted" values from the other values in the abstract domain
      and return the resulting two new Cartesian sets.
    */
    std::pair<CartesianSet, CartesianSet>
    split_domain(int var, const std::vector<int>& wanted) const;

    bool includes(const AbstractState& other) const;
    bool includes(const State& concrete_state) const;
    bool includes(const std::vector<FactPair>& facts) const;

    // IDs are consecutive, so they can be used to index states in vectors.
    int get_id() const;

    NodeID get_node_id() const;

    friend std::ostream&
    operator<<(std::ostream& os, const AbstractState& state)
    {
        return os << "#" << state.get_id() << state.cartesian_set;
    }

    // Create the initial, unrefined abstract state.
    static std::unique_ptr<AbstractState>
    get_trivial_abstract_state(const std::vector<int>& domain_sizes);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
