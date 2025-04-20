#include "downward/cartesian_abstractions/utils.h"

#include "downward/heuristics/additive_heuristic.h"
#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>

using namespace std;

namespace downward::cartesian_abstractions {

static bool operator_applicable(
    const OperatorProxy& op,
    const utils::HashSet<FactPair>& facts)
{
    for (FactProxy precondition : op.get_preconditions()) {
        if (!facts.contains(precondition.get_pair())) return false;
    }
    return true;
}

static bool
operator_achieves_fact(const OperatorProxy& op, const FactProxy& fact)
{
    for (EffectProxy effect : op.get_effects()) {
        if (effect.get_fact() == fact) return true;
    }
    return false;
}

static utils::HashSet<FactPair>
compute_possibly_before_facts(const TaskProxy& task, const FactProxy& last_fact)
{
    utils::HashSet<FactPair> pb_facts;

    // Add facts from initial state.
    for (FactPair fact : task.get_initial_state() | as_fact_pair_set)
        pb_facts.insert(fact);

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
        for (OperatorProxy op : task.get_operators()) {
            // Ignore operators that achieve last_fact.
            if (operator_achieves_fact(op, last_fact)) continue;
            // Add all facts that are achieved by an applicable operator.
            if (operator_applicable(op, pb_facts)) {
                for (EffectProxy effect : op.get_effects()) {
                    pb_facts.insert(effect.get_fact().get_pair());
                }
            }
        }
    }
    return pb_facts;
}

utils::HashSet<FactPair>
get_relaxed_possible_before(const TaskProxy& task, const FactProxy& fact)
{
    utils::HashSet<FactPair> reachable_facts =
        compute_possibly_before_facts(task, fact);
    reachable_facts.insert(fact.get_pair());
    return reachable_facts;
}

vector<int> get_domain_sizes(const VariablesProxy& variables)
{
    vector<int> domain_sizes;
    for (VariableProxy var : variables)
        domain_sizes.push_back(var.get_domain_size());
    return domain_sizes;
}
} // namespace downward::cartesian_abstractions
