#include "probfd/cartesian_abstractions/flaw.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/split_selector.h"

#include "downward/abstract_task.h"

#include <cassert>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

Flaw::Flaw(
    downward::State&& concrete_state,
    const AbstractState& current_abstract_state,
    CartesianSet&& desired_cartesian_set)
    : concrete_state(std::move(concrete_state))
    , current_abstract_state(current_abstract_state)
    , desired_cartesian_set(std::move(desired_cartesian_set))
{
    assert(current_abstract_state.includes(this->concrete_state));
}

vector<Split> Flaw::get_possible_splits(const VariableSpace& variables) const
{
    vector<Split> splits;
    /*
      For each fact in the concrete state that is not contained in the
      desired abstract state, loop over all values in the domain of the
      corresponding variable. The values that are in both the current and
      the desired abstract state are the "wanted" ones, i.e., the ones that
      we want to split off.
    */
    for (VariableProxy var : variables) {
        int var_id = var.get_id();
        int state_value = concrete_state[var_id];
        if (!desired_cartesian_set.test(var_id, state_value)) {
            vector<int> wanted;
            for (int value = 0; value < var.get_domain_size(); ++value) {
                if (current_abstract_state.contains(var_id, value) &&
                    desired_cartesian_set.test(var_id, value)) {
                    wanted.push_back(value);
                }
            }
            assert(!wanted.empty());
            splits.emplace_back(var_id, std::move(wanted));
        }
    }
    assert(!splits.empty());
    return splits;
}

} // namespace probfd::cartesian_abstractions
