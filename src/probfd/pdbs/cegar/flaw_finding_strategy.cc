#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/cegar/flaw.h"

#include "downward/classical_operator_space.h"
#include "downward/goal_fact_list.h"
#include "downward/state.h"

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
    for (const auto& [var, val] : facts) {
        if (state[var] != val) {
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
    OperatorPreconditionsProxy facts,
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
