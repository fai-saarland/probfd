#include "probfd/heuristics/pdbs/cegar/policy_exploration_strategy.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"

#include "probfd/heuristics/pdbs/projection_info.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

namespace {
bool collect_flaws(
    auto facts,
    const State& state,
    bool is_precondition,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaw_list)
{
    bool flaws_found = false;

    // Collect all non-satisfied goal fact variables.
    for (const FactProxy fact : facts) {
        const auto& [var, val] = fact.get_pair();

        if (state[var].get_value() != val) {
            flaws_found = true;
            if (!flaw_filter(var)) {
                flaw_list.emplace_back(var, is_precondition);
            }
        }
    }

    return flaws_found;
}
} // namespace

bool PolicyExplorationStrategy::apply_policy(
    ProbabilisticTaskProxy task_proxy,
    const ProjectionInfo& info,
    const ProjectionMultiPolicy& policy,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaws,
    utils::CountdownTimer& timer)
{
    return this->apply_policy(
        task_proxy,
        *info.mdp,
        *info.pdb,
        policy,
        flaw_filter,
        flaws,
        timer);
}

bool collect_flaws(
    PreconditionsProxy facts,
    const State& state,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaw_list)
{
    return collect_flaws(facts, state, true, flaw_filter, flaw_list);
}

bool collect_flaws(
    GoalsProxy facts,
    const State& state,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaw_list)
{
    return collect_flaws(facts, state, false, flaw_filter, flaw_list);
}

static class PolicyExplorationStrategyCategoryPlugin
    : public plugins::TypedCategoryPlugin<PolicyExplorationStrategy> {
public:
    PolicyExplorationStrategyCategoryPlugin()
        : TypedCategoryPlugin("PolicyExplorationStrategy")
    {
        document_synopsis(
            "Policy exploration strategy for the CEGAR algorithm.");
    }
} _category_plugin;
} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd