#include "probfd/heuristics/cartesian/flaw_generator.h"

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split_selector.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/ta_topological_value_iteration.h"
#include "probfd/engines/trap_aware_dfhs.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/task_utils/task_properties.h"

#include "cegar/abstract_state.h"

#include "utils/countdown_timer.h"
#include "utils/memory.h"

#include "plugins/plugin.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

Flaw::Flaw(
    State&& concrete_state,
    const AbstractState& current_abstract_state,
    CartesianSet&& desired_cartesian_set)
    : concrete_state(std::move(concrete_state))
    , current_abstract_state(current_abstract_state)
    , desired_cartesian_set(std::move(desired_cartesian_set))
{
    assert(current_abstract_state.includes(this->concrete_state));
}

vector<Split> Flaw::get_possible_splits() const
{
    vector<Split> splits;
    /*
      For each fact in the concrete state that is not contained in the
      desired abstract state, loop over all values in the domain of the
      corresponding variable. The values that are in both the current and
      the desired abstract state are the "wanted" ones, i.e., the ones that
      we want to split off.
    */
    for (FactProxy wanted_fact_proxy : concrete_state) {
        FactPair fact = wanted_fact_proxy.get_pair();
        if (!desired_cartesian_set.test(fact.var, fact.value)) {
            VariableProxy var = wanted_fact_proxy.get_variable();
            int var_id = var.get_id();
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

vector<value_t> compute_distances(
    Abstraction& abstraction,
    CartesianHeuristic& heuristic,
    const vector<value_t>& costs)
{
    vector<value_t> values(abstraction.get_num_states(), INFINITE_VALUE);

    CartesianCostFunction cost_function(abstraction, costs);

    preprocessing::QualitativeReachabilityAnalysis<
        const AbstractState*,
        const ProbabilisticTransition*>
        qr_analysis(true);

    std::vector<StateID> pruned_states;
    qr_analysis.run_analysis(
        {abstraction, cost_function},
        &abstraction.get_initial_state(),
        iterators::discarding_output_iterator{},
        std::back_inserter(pruned_states),
        iterators::discarding_output_iterator{});

    for (StateID pruned_id : pruned_states) {
        heuristic.set_h_value(pruned_id, INFINITE_VALUE);
    }

    engines::ta_topological_vi::TATopologicalValueIteration<
        const AbstractState*,
        const ProbabilisticTransition*>
        tvi(&abstraction, &cost_function, &heuristic);

    tvi.solve(abstraction.get_initial_state().get_id(), values);

    return values;
}

static class FlawGeneratorFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<FlawGeneratorFactory> {
public:
    FlawGeneratorFactoryCategoryPlugin()
        : TypedCategoryPlugin("FlawGeneratorFactory")
    {
        document_synopsis("Factory for flaw generation algorithms used in the "
                          "cartesian abstraction "
                          "refinement loop");
    }
} _category_plugin;

} // namespace cartesian
} // namespace heuristics
} // namespace probfd