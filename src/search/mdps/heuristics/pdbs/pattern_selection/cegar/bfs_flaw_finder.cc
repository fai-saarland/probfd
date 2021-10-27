#include "bfs_flaw_finder.h"

#include "pattern_collection_generator_cegar.h"
#include "abstract_solution_data.h"

#include "../../abstract_state.h"
#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../utils/collections.h"

#include <stack>

using namespace std;
using namespace utils;

namespace {
static const std::string token = "CEGAR_PDBs: ";
}

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
std::pair<FlawList, bool> BFSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    FlawList flaws;
    ExplicitGState current(init);

    std::deque<ExplicitGState> open;
    open.push_back(init);
    std::unordered_set<ExplicitGState> closed;
    closed.insert(init.values);

    bool goal_violation = false;

    while (!open.empty()) {
        ExplicitGState current = open.front();
        open.pop_front();

        const AbstractState abs = pdb.get_abstract_state(current.values);
        const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

        // We reached an abstract goal, check if the concrete state is a goal
        if (!abs_op) {
            if (!current.is_goal()) {
                goal_violation = true;

                if (!base.ignore_goal_violations) {
                    // Collect all non-satisfied goal variables that are still
                    // available.
                    for (const auto& [goal_var, goal_value] : g_goal) {
                        if (current[goal_var] != goal_value &&
                            !utils::contains(base.global_blacklist, goal_var) &&
                            utils::contains(base.remaining_goals, goal_var)) {
                            flaws.emplace_back(true, solution_index, goal_var);
                        }
                    }

                    return {flaws, goal_violation};
                }
            }

            continue;
        }

        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperator& op = g_operators[original_id];

        // Check whether all preconditions are fulfilled
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

            if (current[pre_var] != pre_val) {
                flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        if (!flaws.empty()) {
            return {flaws, goal_violation};
        }

        // Generate the successors and add them to the open list
        for (const auto& outcome : op) {
            ExplicitGState successor = current.get_successor(*outcome.op);

            if (!utils::contains(closed, successor)) {
                closed.insert(successor);
                open.push_back(successor);
            }
        }
    }

    assert(flaws.empty());
    return {flaws, goal_violation};
}

template <typename PDBType>
static std::shared_ptr<FlawFindingStrategy<PDBType>>
_parse(options::OptionParser& parser)
{
    Options opts = parser.parse();
    if (parser.dry_run()) return nullptr;

    return make_shared<BFSFlawFinder<PDBType>>();
}

static Plugin<FlawFindingStrategy<MaxProbProjection>>
    _plugin_maxprob("bfs_flaw_finder_mp", _parse<MaxProbProjection>);
static Plugin<FlawFindingStrategy<ExpCostProjection>>
    _plugin_expcost("bfs_flaw_finder_ec", _parse<ExpCostProjection>);

template class BFSFlawFinder<MaxProbProjection>;
template class BFSFlawFinder<ExpCostProjection>;

}
}
}