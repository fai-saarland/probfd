#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/pattern_collection_generator_disjoint_cegar.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/distribution.h"

#include "probfd/task_utils/task_properties.h"

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
    : SamplingFlawFinder(opts.get<int>("violation_threshold"))
{
}

SamplingFlawFinder::SamplingFlawFinder(unsigned violation_threshold)
    : violation_threshold(violation_threshold)
{
}

bool SamplingFlawFinder::apply_policy(
    PatternCollectionGeneratorDisjointCegar& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    std::vector<Flaw>& flaw_list,
    utils::CountdownTimer& timer)
{
    assert(stk.empty() && einfos.empty());

    StateRegistry registry(task_proxy);

    bool executable = true;
    unsigned int violation = 0;

    {
        State init = registry.get_initial_state();
        int status = push_state(
            base,
            task_proxy,
            solution_index,
            init,
            flaw_list,
            registry);
        if ((status & STATE_PUSHED) == 0) {
            assert(stk.empty() && einfos.empty());
            return (status & FLAW_OCCURED) == 0;
        }
    }

    assert(!stk.empty());

    do {
        const State& current = stk.top();
        ExplorationInfo& einfo = einfos[StateID(current.get_id())];

        while (!einfo.successors.empty()) {
            timer.throw_if_expired();

            // Sample next successor
            auto it = einfo.successors.sample(*base.rng);
            auto& succ_id = it->item;

            // Ignore states already seen
            if (!einfos[succ_id].explored) {
                unsigned int status = push_state(
                    base,
                    task_proxy,
                    solution_index,
                    registry.lookup_state(succ_id),
                    flaw_list,
                    registry);

                // Recurse if the state was pushed
                if (status & STATE_PUSHED) {
                    einfo.successors.erase(it);
                    goto continue_exploration;
                }

                // Otherwise, check if the state was not pushed due to a flaw
                if (status & FLAW_OCCURED) {
                    executable = false;

                    if (++violation >= violation_threshold) {
                        goto break_exploration;
                    }
                }
            }

            // Don't sample it again
            einfo.successors.erase(it);
        }

        stk.pop();

    continue_exploration:;
    } while (!stk.empty());

break_exploration:;

    utils::release_container_memory(stk);
    einfos.clear();
    return executable;
}

std::string SamplingFlawFinder::get_name()
{
    return "Sampling Flaw Finder";
}

unsigned int SamplingFlawFinder::push_state(
    PatternCollectionGeneratorDisjointCegar& base,
    const ProbabilisticTaskProxy& task_proxy,
    int solution_index,
    State state,
    std::vector<Flaw>& flaw_list,
    StateRegistry& registry)
{
    auto info = einfos[StateID(state.get_id())];
    assert(!info.explored);
    info.explored = true;

    PDBInfo& solution = *base.pdb_infos[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const ProbabilisticPatternDatabase& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const StateRank abs = pdb.get_abstract_state(state);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return 0;
    }

    const auto abs_operators = policy[abs];

    // We reached a terminal state, check if it is a goal
    if (abs_operators.empty()) {
        assert(solution.is_goal(abs));

        if (!::task_properties::is_goal_state(task_proxy, state)) {
            // Collect all non-satisfied goal variables that are
            // still available.
            for (FactProxy fact : task_proxy.get_goals()) {
                const auto& [goal_var, goal_val] = fact.get_pair();

                if (state[goal_var].get_value() != goal_val &&
                    !base.blacklisted_variables.contains(goal_var) &&
                    utils::contains(base.remaining_goals, goal_var)) {
                    flaw_list.emplace_back(solution_index, goal_var);
                }
            }

            return FLAW_OCCURED;
        }

        return 0;
    }

    std::vector<Flaw> local_flaws;

    const auto operators = task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        const ProbabilisticOperatorProxy op = operators[abs_op->operator_id];

        // Check whether precondition flaws occur
        bool preconditions_ok = true;

        for (const FactProxy precondition : op.get_preconditions()) {
            const auto& [pre_var, pre_val] = precondition.get_pair();

            // We ignore blacklisted variables
            if (base.blacklisted_variables.contains(pre_var)) {
                continue;
            }

            if (state[pre_var].get_value() != pre_val) {
                preconditions_ok = false;
                local_flaws.emplace_back(solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!preconditions_ok) {
            continue; // Try next operator
        }

        // Generate the successors
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            info.successors.add_probability(
                registry.get_successor_state(state, outcome).get_id(),
                outcome.get_probability());
        }

        stk.push(std::move(state));
        return STATE_PUSHED;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return FLAW_OCCURED;
}

static std::shared_ptr<FlawFindingStrategy>
_parse(options::OptionParser& parser)
{
    parser.add_option<int>(
        "violation_threshold",
        "Maximal number of states for which a flaw is tolerated before aborting"
        "the search.",
        "1",
        options::Bounds("1", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<SamplingFlawFinder>(opts);
}

static Plugin<FlawFindingStrategy> _plugin("sampling_flaw_finder", _parse);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd