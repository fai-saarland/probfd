#include "probfd/cartesian_abstractions/abstract_state.h"

#include "probfd/probabilistic_operator_space.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include <cassert>
#include <ostream>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

AbstractState::AbstractState(
    int state_id,
    NodeID node_id,
    CartesianSet cartesian_set)
    : state_id_(state_id)
    , node_id_(node_id)
    , cartesian_set_(std::move(cartesian_set))
{
}

int AbstractState::count(int var) const
{
    return cartesian_set_.count(var);
}

bool AbstractState::contains(int var, int value) const
{
    return cartesian_set_.test(var, value);
}

bool AbstractState::domain_subsets_intersect(
    const AbstractState& other,
    int var) const
{
    return cartesian_set_.intersects(other.cartesian_set_, var);
}

int AbstractState::get_id() const
{
    return state_id_;
}

NodeID AbstractState::get_node_id() const
{
    return node_id_;
}

const CartesianSet& AbstractState::get_cartesian_set() const
{
    return cartesian_set_;
}

std::ostream& operator<<(std::ostream& os, const AbstractState& state)
{
    return os << "#" << state.get_id() << state.cartesian_set_;
}

pair<CartesianSet, CartesianSet> split_cart_state(
    const CartesianSet& cartesian_set,
    int var,
    const vector<int>& wanted)
{
    CartesianSet v1_cartesian_set(cartesian_set);
    CartesianSet v2_cartesian_set(cartesian_set);

    v2_cartesian_set.remove_all(var);
    for (const int value : wanted) {
        // The wanted value has to be in the set of possible values.
        assert(cartesian_set.test(var, value));

        // In v1 var can have all of the previous values except the wanted ones.
        v1_cartesian_set.remove(var, value);

        // In v2 var can only have the wanted values.
        v2_cartesian_set.add(var, value);
    }

    return make_pair(v1_cartesian_set, v2_cartesian_set);
}

CartesianSet regress(
    CartesianSet cartesian_set,
    const ProbabilisticOperatorProxy& op,
    const ProbabilisticEffectsProxy& effects)
{
    for (ProbabilisticEffectProxy effect : effects) {
        const int var_id = effect.get_fact().var;
        cartesian_set.add_all(var_id);
    }

    for (const auto [var, value] : op.get_preconditions()) {
        cartesian_set.set_single_value(var, value);
    }

    return cartesian_set;
}

} // namespace probfd::cartesian_abstractions
