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

namespace {
static const std::string token = "CEGAR_PDBs: ";
}

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
FlawList SamplingFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    const ExplicitGState& init)
{
    AbstractSolutionData<PDBType>& solution =
        *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    FlawList flaws;
    std::stack<ExplicitGState> stk;
    stk.push(init);

    struct ExplorationInfo {
        Distribution<ExplicitGState> successors;
    };

    std::unordered_map<ExplicitGState, ExplorationInfo> einfos;

    while (!stk.empty()) {
        const ExplicitGState& current = stk.top();

        auto p = einfos.try_emplace(current);
        ExplorationInfo& einfo = p.first->second;

        // Already explored
        if (!p.second) {
            stk.pop();
            continue;
        }

        // Check flaws, generate successors
        const AbstractState abs = pdb.get_abstract_state(current.values);
        const AbstractOperator* abs_op = policy.get_operator_if_present(abs);

        // We reached an abstract goal, check if the concrete state is a
        // goal
        if (!abs_op) {
            if (!current.is_goal()) {
                if (base.verbosity >= Verbosity::VERBOSE) {
                    cout << token << "Policy of pattern "
                         << solution.get_pattern()
                         << "failed with goal violation." << std::endl;
                }

                if (!base.ignore_goal_violations) {
                    // Collect all non-satisfied goal variables that are
                    // still available.
                    for (const auto& [goal_var, goal_value] : g_goal) {
                        if (current[goal_var] != goal_value &&
                            !utils::contains(base.global_blacklist, goal_var) &&
                            utils::contains(base.remaining_goals, goal_var)) {
                            flaws.emplace_back(true, solution_index, goal_var);
                        }
                    }

                    return flaws;
                }

                if (base.verbosity >= Verbosity::VERBOSE) {
                    cout << "We ignore goal violations, thus we continue."
                         << endl;
                }
            }

            stk.pop();
            continue;
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

            if (current[pre_var] != pre_val) {
                flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!flaws.empty()) {
            if (base.verbosity >= Verbosity::VERBOSE) {
                cout << token << "Policy of pattern "
                        << solution.get_pattern()
                        << "failed with precondition violation."
                        << std::endl;
            }

            return flaws;
        }

        // Generate the successors
        for (const auto& [det_op, prob] : op) {
            einfo.successors.add_unique(current.get_successor(*det_op), prob);
        }

        // Sample successor and recurse
        auto it = einfo.successors.sample(*base.rng);
        stk.push(std::move(it->first));
        einfo.successors.erase(it);
    }

    assert (flaws.empty());
    return flaws;
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