#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/cegar/cegar.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "probfd/distribution.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"
#include "utils/rng.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

#include <stack>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SamplingFlawFinder::SamplingFlawFinder(options::Options& opts)
    : SamplingFlawFinder(opts.get<int>("max_search_states"))
{
}

SamplingFlawFinder::SamplingFlawFinder(int max_search_states)
    : max_search_states(max_search_states)
{
}

bool SamplingFlawFinder::apply_policy(
    const CEGAR& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
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

    const PDBInfo& solution = *base.pdb_infos[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const ProbabilisticPatternDatabase& pdb = solution.get_pdb();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy goals = task_proxy.get_goals();

    for (;;) {
        const State* current = &stk.back();
        const StateRank abs = pdb.get_abstract_state(*current);

        ExplorationInfo* einfo = &einfos[StateID(current->get_id())];

        assert(!einfo->explored);
        einfo->explored = true;

        {
            // We reached a dead-end, the operator is irrelevant.
            if (pdb.is_dead_end(abs)) {
                stk.pop_back();
                goto backtrack;
            }

            const auto& abs_operators = policy[abs];

            // Goal flaw check
            if (abs_operators.empty()) {
                assert(solution.is_goal(abs));

                if (base.collect_flaws(
                        goals,
                        *current,
                        solution_index,
                        flaw_list)) {
                    return false;
                }

                goto backtrack;
            }

            std::vector<Flaw> local_flaws;

            // Precondition flaw check
            for (const AbstractOperator* abs_op : abs_operators) {
                const auto op = operators[abs_op->operator_id];

                if (base.collect_flaws(
                        op.get_preconditions(),
                        *current,
                        solution_index,
                        local_flaws)) {
                    continue; // Try next operator
                }

                // Generate the successors
                for (const auto outcome : op.get_outcomes()) {
                    const State successor =
                        registry.get_successor_state(*current, outcome);

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
                auto it = einfo->successors.sample(*base.rng);
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

    abort();
}

std::string SamplingFlawFinder::get_name()
{
    return "Sampling Flaw Finder";
}

static std::shared_ptr<FlawFindingStrategy>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "max_search_states",
        "Maximal number of generated states after which the flaw search is "
        "aborted.",
        "20M",
        options::Bounds("0", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<SamplingFlawFinder>(opts);
}

static Plugin<FlawFindingStrategy> _plugin("sampling_flaw_finder", _parse);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd