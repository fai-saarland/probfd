#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/task_proxy.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"

class AbstractTask;

using namespace std;

namespace probfd::cartesian_abstractions {

unique_ptr<additive_heuristic::AdditiveHeuristic>
create_additive_heuristic(const shared_ptr<ProbabilisticTask>& task)
{
    return std::make_unique<additive_heuristic::AdditiveHeuristic>(
        std::make_shared<tasks::DeterminizationTask>(task),
        false,
        "",
        utils::Verbosity::SILENT);
}

static bool operator_applicable(
    const ProbabilisticOperatorProxy& op,
    const utils::HashSet<FactPair>& facts)
{
    return std::ranges::all_of(
        op.get_preconditions() | views::transform(&FactProxy::get_pair),
        [&](const FactPair& precondition) {
            return facts.contains(precondition);
        });
}

static bool outcome_can_achieve_fact(
    const ProbabilisticOutcomeProxy& outcome,
    const FactPair& fact)
{
    return std::ranges::any_of(
        outcome.get_effects() |
            views::transform(&ProbabilisticEffectProxy::get_fact),
        [&](FactProxy effect) { return effect.get_pair() == fact; });
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

} // namespace probfd::cartesian_abstractions
