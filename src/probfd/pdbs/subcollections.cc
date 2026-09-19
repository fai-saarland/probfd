#include "probfd/pdbs/subcollections.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/pdbs/pattern_cliques.h"

#include "downward/fact_pair.h"
#include "downward/variable_space.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <unordered_set>
#include <utility>

using namespace downward;

namespace probfd::pdbs {

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    const VariableSpace& variables,
    const ProbabilisticOperatorSpace& operators,
    bool ignore_deterministic)
{
    const size_t num_vars = variables.size();

    std::vector are_orthogonal(num_vars, std::vector(num_vars, true));

    for (const ProbabilisticOperatorProxy& op : operators) {
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

        if (ignore_deterministic && outcomes.size() == 1) {
            continue;
        }

        std::unordered_set<int> affected_vars;

        for (const ProbabilisticOutcomeProxy& outcome : outcomes) {
            for (const auto& effect : outcome.get_effects()) {
                const int new_var = effect.get_fact().var;
                if (affected_vars.insert(new_var).second) {
                    are_orthogonal[new_var][new_var] = false;
                    for (const int old_var : affected_vars) {
                        if (old_var == new_var) continue;
                        are_orthogonal[old_var][new_var] = false;
                        are_orthogonal[new_var][old_var] = false;
                    }
                }
            }
        }
    }

    return are_orthogonal;
}

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const VariableSpace& variables,
    const ProbabilisticOperatorSpace& operators,
    const PatternCollection& patterns,
    bool ignore_deterministic)
{
    return build_compatibility_graph_orthogonality(
        patterns,
        compute_prob_orthogonal_vars(
            variables,
            operators,
            ignore_deterministic));
}

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const std::vector<std::vector<bool>>& var_orthogonality)
{
    using ::pdbs::are_patterns_additive;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (are_patterns_additive(
                    patterns[i],
                    patterns[j],
                    var_orthogonality)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

} // namespace probfd::pdbs
