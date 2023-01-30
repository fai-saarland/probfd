#include "probfd/heuristics/pdbs/subcollections/orthogonality.h"

#include "pdbs/pattern_cliques.h"

#include "probfd/task_proxy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

std::vector<int> get_affected_vars(
    const ProbabilisticOperatorProxy& op,
    bool ignore_deterministic)
{
    ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

    std::vector<int> affected_vars;

    if (ignore_deterministic) {
        if (outcomes.size() == 1) {
            return affected_vars;
        }
    }

    // Compute the variables that may be changed by the operator
    for (const ProbabilisticOutcomeProxy& outcome : op.get_outcomes()) {
        const auto effects = outcome.get_effects();

        for (const ProbabilisticEffectProxy& effect : effects) {
            affected_vars.push_back(effect.get_fact().get_variable().get_id());
        }
    }

    return affected_vars;
}

VariableOrthogonality compute_prob_orthogonal_vars(
    const ProbabilisticTaskProxy& task_proxy,
    bool ignore_deterministic)
{
    const VariablesProxy variables = task_proxy.get_variables();
    const size_t num_vars = variables.size();

    VariableOrthogonality are_orthogonal(
        num_vars,
        std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperatorProxy& op : task_proxy.get_operators()) {
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
    const ProbabilisticTaskProxy& task_proxy,
    const PatternCollection& patterns,
    bool ignore_deterministic)
{
    return build_compatibility_graph_orthogonality(
        patterns,
        compute_prob_orthogonal_vars(task_proxy, ignore_deterministic));
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
} // namespace heuristics
} // namespace probfd
