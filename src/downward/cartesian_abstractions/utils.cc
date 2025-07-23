#include "downward/cartesian_abstractions/utils.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/state.h"

#include <ranges>

using namespace std;

namespace downward::cartesian_abstractions {

static bool operator_applicable(
    const OperatorProxy& op,
    const utils::HashSet<FactPair>& facts)
{
    for (FactPair precondition : op.get_preconditions()) {
        if (!facts.contains(precondition)) return false;
    }
    return true;
}

static bool operator_achieves_fact(const OperatorProxy& op, FactPair fact)
{
    for (auto effect : op.get_effects()) {
        if (effect.get_fact() == fact) return true;
    }
    return false;
}

static utils::HashSet<FactPair> compute_possibly_before_facts(
    const ClassicalOperatorSpace& operators,
    const State& state,
    FactPair last_fact)
{
    utils::HashSet<FactPair> pb_facts;

    // Add facts from initial state.
    for (FactPair fact : state | as_fact_pair_set) pb_facts.insert(fact);

    // Until no more facts can be added:
    size_t last_num_reached = 0;
    /*
      Note: This can be done more efficiently by maintaining the number
      of unsatisfied preconditions for each operator and a queue of
      unhandled effects.

      TODO: Find out if this code is time critical, and change it if it
      is.
    */
    while (last_num_reached != pb_facts.size()) {
        last_num_reached = pb_facts.size();
        for (OperatorProxy op : operators) {
            // Ignore operators that achieve last_fact.
            if (operator_achieves_fact(op, last_fact)) continue;
            // Add all facts that are achieved by an applicable operator.
            if (operator_applicable(op, pb_facts)) {
                for (auto effect : op.get_effects()) {
                    pb_facts.insert(effect.get_fact());
                }
            }
        }
    }
    return pb_facts;
}

utils::HashSet<FactPair> get_relaxed_possible_before(
    const ClassicalOperatorSpace& operators,
    const State& state,
    FactPair fact)
{
    utils::HashSet<FactPair> reachable_facts =
        compute_possibly_before_facts(operators, state, fact);
    reachable_facts.insert(fact);
    return reachable_facts;
}

vector<int> get_domain_sizes(const VariableSpace& variables)
{
    return variables | std::views::transform(&VariableProxy::get_domain_size) |
           std::ranges::to<std::vector>();
}
} // namespace downward::cartesian_abstractions
