#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "downward/task_proxy.h"

using namespace downward;

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
        if (const auto& [var, val] = fact.get_pair(); state[var] != val) {
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

} // namespace probfd::pdbs::cegar
