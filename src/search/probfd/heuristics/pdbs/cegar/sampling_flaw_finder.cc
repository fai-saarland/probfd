#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "probfd/distribution.h"
#include "probfd/multi_policy.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

#include <stack>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SamplingFlawFinder::SamplingFlawFinder(const plugins::Options& opts)
    : SamplingFlawFinder(
          utils::parse_rng_from_options(opts),
          opts.get<int>("max_search_states"))
{
}

SamplingFlawFinder::SamplingFlawFinder(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    int max_search_states)
    : rng(std::move(rng))
    , max_search_states(max_search_states)
{
}

bool SamplingFlawFinder::apply_policy(
    const ProbabilisticTaskProxy& task_proxy,
    const ProjectionStateSpace& mdp,
    const ProbabilityAwarePatternDatabase& pdb,
    const ProjectionMultiPolicy& policy,
    const std::unordered_set<int>& blacklisted_variables,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(stk.empty() && einfos.empty());

    // Exception safety due to TimeoutException
    scope_exit guard([&] {
        stk.clear();
        einfos.clear();
    });

    StateRegistry registry(task_proxy);
    stk.push_back(registry.get_initial_state());

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    for (;;) {
        const State* current = &stk.back();
        const StateRank abs = pdb.get_abstract_state(*current);

        ExplorationInfo* einfo = &einfos[StateID(current->get_id())];

        assert(!einfo->explored);
        einfo->explored = true;

        {
            const std::vector abs_decisions = policy.get_decisions(abs);

            // Goal flaw check
            if (abs_decisions.empty()) {
                if (mdp.is_goal(abs) && collect_flaws(
                                            goals,
                                            *current,
                                            blacklisted_variables,
                                            flaw_list)) {
                    return false;
                }

                goto backtrack;
            }

            std::vector<Flaw> local_flaws;

            // Precondition flaw check
            for (const auto& decision : abs_decisions) {
                const auto* abs_op = decision.action;
                const auto op = operators[abs_op->operator_id];

                if (collect_flaws(
                        op.get_preconditions(),
                        *current,
                        blacklisted_variables,
                        local_flaws)) {
                    continue; // Try next operator
                }

                // Generate the successors
                for (const auto outcome : op.get_outcomes()) {
                    const State successor = registry.get_successor_state(
                        *current,
                        outcome.get_effects());

                    if (static_cast<int>(registry.size()) > max_search_states) {
                        return false;
                    }

                    einfo->successors.add_probability(
                        successor.get_id(),
                        outcome.get_probability());
                }

                goto operator_found;
            }

            // Insert all flaws of all operators
            flaw_list.insert(
                flaw_list.end(),
                local_flaws.begin(),
                local_flaws.end());

            return false;
        }

    operator_found:;

        for (;;) {
            while (!einfo->successors.empty()) {
                timer.throw_if_expired();

                // Sample next successor
                auto it = einfo->successors.sample(*rng);
                const StateID succ_id = it->item;
                einfo->successors.erase(it);

                State successor = registry.lookup_state(succ_id);

                // Ignore states already seen
                if (!einfos[succ_id].explored) {
                    stk.push_back(std::move(successor));
                    goto continue_exploration;
                }
            }

        backtrack:;

            do {
                stk.pop_back();

                if (stk.empty()) {
                    return true;
                }

                current = &stk.back();
                einfo = &einfos[StateID(current->get_id())];
            } while (einfo->successors.empty());
        }

    continue_exploration:;
    }
}

std::string SamplingFlawFinder::get_name()
{
    return "Sampling Flaw Finder";
}

class SamplingFlawFinderFeature
    : public plugins::TypedFeature<FlawFindingStrategy, SamplingFlawFinder> {
public:
    SamplingFlawFinderFeature()
        : TypedFeature("sampling_flaw_finder")
    {
        utils::add_rng_options(*this);
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }
};

static plugins::FeaturePlugin<SamplingFlawFinderFeature> _plugin;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd