#include "downward/cartesian_abstractions/abstract_state.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"
#include "downward/cartesian_abstractions/utils.h"

#include "downward/classical_operator_space.h"
#include "downward/state.h"

#include <cassert>

using namespace std;

namespace downward::cartesian_abstractions {
AbstractState::AbstractState(
    int state_id,
    NodeID node_id,
    CartesianSet&& cartesian_set)
    : state_id(state_id)
    , node_id(node_id)
    , cartesian_set(std::move(cartesian_set))
{
}

int AbstractState::count(int var) const
{
    return cartesian_set.count(var);
}

bool AbstractState::contains(int var, int value) const
{
    return cartesian_set.test(var, value);
}

pair<CartesianSet, CartesianSet>
AbstractState::split_domain(int var, const vector<int>& wanted) const
{
    int num_wanted = wanted.size();
    (void)num_wanted;
    // We can only refine for variables with at least two values.
    assert(num_wanted >= 1);
    assert(cartesian_set.count(var) > num_wanted);

    CartesianSet v1_cartesian_set(cartesian_set);
    CartesianSet v2_cartesian_set(cartesian_set);

    v2_cartesian_set.remove_all(var);
    for (int value : wanted) {
        // The wanted value has to be in the set of possible values.
        assert(cartesian_set.test(var, value));

        // In v1 var can have all of the previous values except the wanted ones.
        v1_cartesian_set.remove(var, value);

        // In v2 var can only have the wanted values.
        v2_cartesian_set.add(var, value);
    }
    assert(
        v1_cartesian_set.count(var) == cartesian_set.count(var) - num_wanted);
    assert(v2_cartesian_set.count(var) == num_wanted);
    return make_pair(v1_cartesian_set, v2_cartesian_set);
}

CartesianSet AbstractState::regress(const OperatorProxy& op) const
{
    CartesianSet regression = cartesian_set;
    for (auto effect : op.get_effects()) {
        regression.add_all(effect.get_fact().var);
    }
    for (const auto [var, value] : op.get_preconditions()) {
        regression.set_single_value(var, value);
    }
    return regression;
}

bool AbstractState::domain_subsets_intersect(
    const AbstractState& other,
    int var) const
{
    return cartesian_set.intersects(other.cartesian_set, var);
}

bool AbstractState::includes(const State& concrete_state) const
{
    for (FactPair fact : concrete_state | as_fact_pair_set) {
        if (!cartesian_set.test(fact.var, fact.value))
            return false;
    }
    return true;
}

bool AbstractState::includes(const vector<FactPair>& facts) const
{
    for (const FactPair& fact : facts) {
        if (!cartesian_set.test(fact.var, fact.value)) return false;
    }
    return true;
}

bool AbstractState::includes(const AbstractState& other) const
{
    return cartesian_set.is_superset_of(other.cartesian_set);
}

int AbstractState::get_id() const
{
    return state_id;
}

NodeID AbstractState::get_node_id() const
{
    return node_id;
}

unique_ptr<AbstractState>
AbstractState::get_trivial_abstract_state(const vector<int>& domain_sizes)
{
    return std::make_unique<AbstractState>(0, 0, CartesianSet(domain_sizes));
}
} // namespace cartesian_abstractions
