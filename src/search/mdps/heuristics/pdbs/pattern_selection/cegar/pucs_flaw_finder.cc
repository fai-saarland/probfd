#include "pucs_flaw_finder.h"

#include "pattern_collection_generator_cegar.h"
#include "abstract_solution_data.h"

#include "../../abstract_state.h"
#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../utils/collections.h"

using namespace std;
using namespace utils;

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
std::pair<FlawList, bool> PUCSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    assert(pq.empty() && probabilities.empty());

    // Push initial state for expansion
    pq.push(1.0, init);
    probabilities[init] = 1.0;

    FlawList flaw_list;

    while (!pq.empty()) {
        const auto& [priority, current] = pq.pop();

        // TODO remove this once we have a real priority queue...
        if (priority < probabilities[current]) {
            continue;
        }

        unsigned int status = expand(
            base,
            solution_index,
            current,
            priority,
            flaw_list);

        if ((status & SUCCESSFULLY_EXPANDED) == 0) {
            // A flaw occured.
            assert(!flaw_list.empty() || status & GOAL_VIOLATED);
            pq.clear();
            probabilities.clear();
            return { flaw_list, status & GOAL_VIOLATED };
        }
    }

    assert(flaw_list.empty());

    pq.clear();
    probabilities.clear();
    return { flaw_list, false };
}

template <typename PDBType>
unsigned int PUCSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    ExplicitGState state,
    value_type::value_t priority,
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

        return SUCCESSFULLY_EXPANDED;
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
        return 0;
    }

    // Generate the successors and add them to the queue
    for (const auto& [det_op, prob] : op) {
        const value_type::value_t succ_prob = priority * prob;
        auto successor = state.get_successor(*det_op);

        auto [it, inserted] = probabilities.emplace(successor, succ_prob);

        if (!inserted) {
            if (succ_prob <= it->second) {
                continue;
            }

            it->second = succ_prob;
        }

        pq.push(succ_prob, std::move(successor));
    }

    return SUCCESSFULLY_EXPANDED;
}

template <typename PDBType>
static std::shared_ptr<FlawFindingStrategy<PDBType>>
_parse(options::OptionParser& parser)
{
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<PUCSFlawFinder<PDBType>>();
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("pucs_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("pucs_flaw_finder_ec", _parse<ExpCostProjection>);

template class PUCSFlawFinder<MaxProbProjection>;
template class PUCSFlawFinder<ExpCostProjection>;

}
}
}