#include "sampling_flaw_finder.h"

#include "pattern_collection_generator_cegar.h"
#include "abstract_solution_data.h"

#include "../../abstract_state.h"
#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../distribution.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../utils/collections.h"
#include "../../../../../utils/rng.h"

#include <stack>

using namespace std;
using namespace utils;

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
std::pair<FlawList, bool> SamplingFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    assert (stk.empty() && einfos.empty());

    FlawList flaw_list;

    int status = push_state(base, solution_index, init, flaw_list);
    if ((status & STATE_PUSHED) == 0) {
        assert(stk.empty() && einfos.empty());
        return { flaw_list, status & GOAL_VIOLATED };
    }

    assert (!stk.empty());

    do {
        const ExplicitGState& current = stk.top();
        ExplorationInfo& einfo = einfos[current];

        while (!einfo.successors.empty()) {
            // Sample next successor
            auto it = einfo.successors.sample(*base.rng);
            auto& succ = it->first;

            // Ignore states already seen
            if (einfos.find(succ) == einfos.end()) {
                unsigned int status = push_state(
                    base,
                    solution_index,
                    std::move(succ),
                    flaw_list);

                // Recurse if the state was pushed
                if (status & STATE_PUSHED) {
                    einfo.successors.erase(it);
                    goto continue_exploration;
                }

                // Otherwise, check if the state was not pushed due to a flaw
                if (status & FLAW_OCCURED) {
                    assert(!flaw_list.empty() || status & GOAL_VIOLATED);
                    std::stack<ExplicitGState>().swap(stk); // Clear stack 
                    einfos.clear();
                    return { flaw_list, status & GOAL_VIOLATED };
                }
            }

            // Don't sample it again
            einfo.successors.erase(it);
        }

        stk.pop();

        continue_exploration:;
    } while (!stk.empty());

    assert (stk.empty());

    einfos.clear();
    return { flaw_list, false };
}

template <typename PDBType>
unsigned int SamplingFlawFinder<PDBType>::push_state(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    ExplicitGState state,
    FlawList& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const AbstractState abs = pdb.get_abstract_state(state.values);
    const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

    // We reached an abstract goal, check if the concrete state is a
    // goal
    if (!abs_op) {
        if (!state.is_goal()) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are
                // still available.
                for (const auto& [goal_var, goal_value] : g_goal) {
                    if (state[goal_var] != goal_value &&
                        !utils::contains(base.global_blacklist, goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return GOAL_VIOLATED;
        }

        return 0;
    }

    int original_id = abs_op->original_operator_id;
    const ProbabilisticOperator& op = g_operators[original_id];

    // Check whether precondition flaws occur
    for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
        // We ignore blacklisted variables
        const bool is_blacklist_var =
            utils::contains(base.global_blacklist, pre_var);

        if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
            assert(
                !solution.is_blacklisted(pre_var) ||
                base.local_blacklisting);
            continue;
        }

        if (state[pre_var] != pre_val) {
            flaw_list.emplace_back(false, solution_index, pre_var);
        }
    }

    // Flaws occured.
    if (!flaw_list.empty()) {
        return PRECONDITION_VIOLATED;
    }

    // Generate the successors
    ExplorationInfo& einfo = einfos[state];
    for (const auto& [det_op, prob] : op) {
        einfo.successors.add_unique(state.get_successor(*det_op), prob);
    }

    stk.push(std::move(state));

    return STATE_PUSHED;
}

template <typename PDBType>
static std::shared_ptr<FlawFindingStrategy<PDBType>>
_parse(options::OptionParser& parser)
{
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<SamplingFlawFinder<PDBType>>();
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("sampling_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("sampling_flaw_finder_ec", _parse<ExpCostProjection>);

template class SamplingFlawFinder<MaxProbProjection>;
template class SamplingFlawFinder<ExpCostProjection>;

}
}
}