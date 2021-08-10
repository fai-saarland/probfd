#include "orthogonality.h"

#include "../../../../pdbs/pattern_cliques.h"
#include "../../../globals.h"
#include "../../../probabilistic_operator.h"

namespace probabilistic {
namespace pdbs {

std::vector<int>
get_affected_vars(const ProbabilisticOperator& op, bool ignore_deterministic)
{
    std::vector<int> affected_vars;

    if (ignore_deterministic) {
        if (!op.is_stochastic()) {
            return affected_vars;
        }
    }

    // Compute the variables that may be changed by the operator
    for (const ProbabilisticOutcome& outcome : op) {
        const auto effects = outcome.op->get_effects();

        for (const ::GlobalEffect& eff : outcome.op->get_effects()) {
            affected_vars.push_back(eff.var);
        }
    }

    return affected_vars;
}

VariableOrthogonality compute_prob_orthogonal_vars(bool ignore_deterministic)
{
    const std::size_t num_vars = g_variable_domain.size();

    VariableOrthogonality are_orthogonal(
        num_vars,
        std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperator& op : g_operators) {
        const std::vector<int> affected_vars =
            get_affected_vars(op, ignore_deterministic);

        for (unsigned i1 = 0; i1 < affected_vars.size(); i1++) {
            int var1 = affected_vars[i1];
            are_orthogonal[var1][var1] = false;
            for (unsigned i2 = i1 + 1; i2 < affected_vars.size(); i2++) {
                int var2 = affected_vars[i2];
                are_orthogonal[var1][var2] = false;
                are_orthogonal[var2][var1] = false;
            }
        }
    }

    return are_orthogonal;
}

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    bool ignore_deterministic)
{
    return build_compatibility_graph_orthogonality(
        patterns,
        compute_prob_orthogonal_vars(ignore_deterministic));
}

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const VariableOrthogonality& on_vars)
{
    using ::pdbs::are_patterns_additive;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (are_patterns_additive(patterns[i], patterns[j], on_vars)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

} // namespace pdbs
} // namespace probabilistic
