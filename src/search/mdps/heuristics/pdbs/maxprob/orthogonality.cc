#include "orthogonality.h"

#include "../../../../pdbs/pattern_cliques.h"
#include "../../../globals.h"
#include "../../../probabilistic_operator.h"
#include "../syntactic_projection.h"

namespace probabilistic {
namespace pdbs {
namespace multiplicativity {

namespace {
template <typename T>
bool are_disjoint(const std::vector<T>& A, const std::vector<T>& B)
{
    std::vector<T> intersection;

    std::set_intersection(
        A.cbegin(),
        A.cend(),
        B.cbegin(),
        B.cend(),
        std::back_inserter(intersection));

    return intersection.empty();
}
} // namespace

using namespace syntactic_projection;

using VariableOrthogonality = std::vector<std::vector<bool>>;

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns)
{
    using namespace syntactic_projection;
    using OperatorID = std::size_t;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    std::vector<std::vector<OperatorID>> prob_operators;
    prob_operators.resize(patterns.size());

    // Compute operators that are probabilistic when projected for each pattern
    for (std::size_t i = 0; i != g_operators.size(); ++i) {
        const ProbabilisticOperator& op = g_operators[i];

        // A lot of actions are already deterministic in the first place, so
        // we can save some work
        if (!op.is_stochastic()) {
            continue;
        }

        for (std::size_t j = 0; j != patterns.size(); ++j) {
            const Pattern& pattern = patterns[j];

            // Get the syntactically projected operator
            const auto& abs_op = project_operator(pattern, op);

            // If the operator is "truly stochastic" add it to the set
            if (abs_op.is_stochastic() && !abs_op.is_pseudo_deterministic()) {
                prob_operators[j].push_back(i);
            }
        }
    }

    // There is an edge from pattern i to j if they don't have common operators
    // that are probabilistic when projected
    for (std::size_t i = 0; i != patterns.size(); ++i) {
        const auto& prob_operators_i = prob_operators[i];

        for (std::size_t j = i + 1; j != patterns.size(); ++j) {
            const auto& prob_operators_j = prob_operators[j];

            if (are_disjoint(prob_operators_i, prob_operators_j)) {
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

template <bool ignore_deterministic>
std::vector<int> get_affected_vars(const ProbabilisticOperator& op)
{
    std::vector<int> affected_vars;

    if constexpr (ignore_deterministic) {
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

template <bool ignore_deterministic>
VariableOrthogonality compute_prob_orthogonal_vars()
{
    const std::size_t num_vars = g_variable_domain.size();

    VariableOrthogonality are_orthogonal(
        num_vars,
        std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperator& op : g_operators) {
        const std::vector<int> affected_vars =
            get_affected_vars<ignore_deterministic>(op);

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

template <bool ignore_deterministic>
std::vector<std::vector<int>>
build_compatibility_graph_orthogonality(const PatternCollection& patterns)
{
    using ::pdbs::are_patterns_additive;

    VariableOrthogonality pairwise =
        compute_prob_orthogonal_vars<ignore_deterministic>();

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (are_patterns_additive(patterns[i], patterns[j], pairwise)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

// Explicit instantiations to avoid putting this in the header.
template std::vector<std::vector<int>>
build_compatibility_graph_orthogonality<true>(
    const PatternCollection& patterns);

template std::vector<std::vector<int>>
build_compatibility_graph_orthogonality<false>(
    const PatternCollection& patterns);

} // namespace multiplicativity
} // namespace pdbs
} // namespace probabilistic
