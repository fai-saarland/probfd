#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/tasks/all_outcomes_determinization.h"

#include "probfd/task_proxy.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/plugins/options.h"
#include "downward/utils/logging.h"

#include <cstddef>

class AbstractTask;

using namespace std;

namespace probfd {
namespace cartesian_abstractions {

unique_ptr<additive_heuristic::AdditiveHeuristic>
create_additive_heuristic(const shared_ptr<ProbabilisticTask>& task)
{
    plugins::Options opts;
    opts.set<shared_ptr<AbstractTask>>(
        "transform",
        std::make_shared<tasks::AODDeterminizationTask>(task.get()));
    opts.set<bool>("cache_estimates", false);
    opts.set<utils::Verbosity>("verbosity", utils::Verbosity::SILENT);
    return std::make_unique<additive_heuristic::AdditiveHeuristic>(opts);
}

static bool operator_applicable(
    const ProbabilisticOperatorProxy& op,
    const utils::HashSet<FactPair>& facts)
{
    for (FactProxy precondition : op.get_preconditions()) {
        if (!facts.contains(precondition.get_pair())) return false;
    }
    return true;
}

static bool outcome_can_achieve_fact(
    const ProbabilisticOutcomeProxy& outcome,
    const FactPair& fact)
{
    for (ProbabilisticEffectProxy effect : outcome.get_effects()) {
        if (effect.get_fact().get_pair() == fact) return true;
    }
    return false;
}

static utils::HashSet<FactPair> compute_possibly_before_facts(
    const ProbabilisticTaskProxy& task,
    const FactPair& last_fact)
{
    utils::HashSet<FactPair> pb_facts;

    // Add facts from initial state.
    for (FactProxy fact : task.get_initial_state())
        pb_facts.insert(fact.get_pair());

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
        for (ProbabilisticOperatorProxy op : task.get_operators()) {
            if (!operator_applicable(op, pb_facts)) continue;
            for (const auto outcome : op.get_outcomes()) {
                // Ignore outcomes that achieve last_fact.
                if (outcome_can_achieve_fact(outcome, last_fact)) continue;
                // Add all facts that are achieved by an applicable
                // operator.
                for (ProbabilisticEffectProxy effect : outcome.get_effects()) {
                    pb_facts.insert(effect.get_fact().get_pair());
                }
            }
        }
    }
    return pb_facts;
}

utils::HashSet<FactPair> get_relaxed_possible_before(
    const ProbabilisticTaskProxy& task,
    const FactPair& fact)
{
    utils::HashSet<FactPair> reachable_facts =
        compute_possibly_before_facts(task, fact);
    reachable_facts.insert(fact);
    return reachable_facts;
}

} // namespace cartesian_abstractions
} // namespace probfd