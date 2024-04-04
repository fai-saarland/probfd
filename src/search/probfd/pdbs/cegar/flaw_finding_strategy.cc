#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "downward/plugins/plugin.h"

#include "downward/task_proxy.h"

namespace probfd::pdbs::cegar {

namespace {
bool collect_flaws_(
    const auto& facts,
    const State& state,
    std::vector<Flaw>& flaws,
    bool is_precondition,
    const std::function<bool(const Flaw&)>& accept_flaw)
{
    bool flaws_found = false;

    // Collect all non-satisfied goal fact variables.
    for (const FactProxy fact : facts) {
        const auto& [var, val] = fact.get_pair();

        if (state[var].get_value() != val) {
            Flaw flaw(var, is_precondition);
            if (accept_flaw(flaw)) {
                flaws_found = true;
                flaws.push_back(flaw);
            }
        }
    }

    return flaws_found;
}
} // namespace

bool collect_flaws(
    PreconditionsProxy facts,
    const State& state,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw)
{
    return collect_flaws_(facts, state, flaws, true, accept_flaw);
}

bool collect_flaws(
    GoalsProxy facts,
    const State& state,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw)
{
    return collect_flaws_(facts, state, flaws, false, accept_flaw);
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

} // namespace probfd::pdbs::cegar
