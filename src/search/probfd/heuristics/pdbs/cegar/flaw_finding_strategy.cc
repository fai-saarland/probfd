#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"

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
    auto&& ignore_flaw,
    std::vector<Flaw>& flaw_list)
{
    bool flaws_found = false;

    // Collect all non-satisfied goal fact variables.
    for (const FactProxy fact : facts) {
        const auto& [var, val] = fact.get_pair();

        if (state[var].get_value() != val && !ignore_flaw(var)) {
            flaws_found = true;
            flaw_list.emplace_back(var, is_precondition);
        }
    }

    return flaws_found;
}

} // namespace

bool collect_flaws(
    PreconditionsProxy facts,
    const State& state,
    std::function<bool(int)> ignore_flaw,
    std::vector<Flaw>& flaw_list)
{
    return collect_flaws(facts, state, true, ignore_flaw, flaw_list);
}

bool collect_flaws(
    GoalsProxy facts,
    const State& state,
    std::function<bool(int)> ignore_flaw,
    std::vector<Flaw>& flaw_list)
{
    return collect_flaws(facts, state, false, ignore_flaw, flaw_list);
}

static class FlawFindingStrategyCategoryPlugin
    : public plugins::TypedCategoryPlugin<FlawFindingStrategy> {
public:
    FlawFindingStrategyCategoryPlugin()
        : TypedCategoryPlugin("FlawFindingStrategy")
    {
        document_synopsis("Policy CEGAR flaw finding strategy.");
    }
} _category_plugin;
}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd