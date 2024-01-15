#ifndef PROBFD_CARTESIAN_ABSTRACT_STATE_H
#define PROBFD_CARTESIAN_ABSTRACT_STATE_H

#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/cartesian_set.h"

#include <iosfwd>
#include <memory>
#include <utility>
#include <vector>

struct FactPair;
class State;

namespace probfd {
class ProbabilisticOperatorProxy;
class ProbabilisticEffectsProxy;

namespace cartesian_abstractions {

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
    AbstractState& operator=(const AbstractState&) = delete;

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
    operator<<(std::ostream& os, const AbstractState& state);

    // Create the initial, unrefined abstract state.
    static std::unique_ptr<AbstractState>
    get_trivial_abstract_state(const std::vector<int>& domain_sizes);
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_ABSTRACT_STATE_H
