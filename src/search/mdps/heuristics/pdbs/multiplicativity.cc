#include "multiplicativity.h"
#include "../../../pdbs/pattern_cliques.h"

namespace probabilistic {
namespace pdbs{
namespace multiplicativity {

using namespace syntactic_projection;

using VariableOrthogonality = std::vector<std::vector<bool>>;

namespace {
template <typename T>
bool areDisjoint(const std::set<T>& A, const std::set<T>& B) {
    std::vector<T> intersection;

    std::set_intersection(A.cbegin(), A.cend(), B.cbegin(), B.cend(), std::back_inserter(intersection));

    return intersection.empty();
}

bool isPseudoDeterministic(probabilistic::pdbs::syntactic_projection::SyntacticProjection op) {
    if (op.size() == 2) {
        auto it = op.begin();

        if (it->first.empty() || (++it)->first.empty()) {
            return true;
        }
    }

    return false;
}

std::vector<int> get_affected_vars(const ProbabilisticOperator& op) {
    std::vector<int> affected_vars;

    if (!op.is_stochastic()) {
        return affected_vars;
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

VariableOrthogonality compute_prob_orthogonal_vars() {
    const std::size_t num_vars = g_variable_domain.size();

    VariableOrthogonality are_orthogonal(num_vars, std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperator& op : g_operators) {
        const std::vector<int> affected_vars = get_affected_vars(op);

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

}


std::vector<std::vector<int>> buildCompatibilityGraphOrthogonality(
    const ::pdbs::PatternCollection& patterns)
{
    VariableOrthogonality are_orthogonal = compute_prob_orthogonal_vars();

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (::pdbs::are_patterns_additive(patterns[i], patterns[j], are_orthogonal)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

std::vector<std::vector<int>> buildCompatibilityGraphWeakOrthogonality(
    const ::pdbs::PatternCollection& patterns)
{
    using namespace syntactic_projection;
    using OperatorID = std::size_t;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    std::vector<std::set<OperatorID>> pattern2proboperators;
    pattern2proboperators.resize(patterns.size());

    // Compute operators that are probabilistic when projected for each pattern
    for (std::size_t i = 0; i != g_operators.size(); ++i) {
        const ProbabilisticOperator& op = g_operators[i];

        // A lot of actions are already deterministic in the first place, so we can save some work
        if (!op.is_stochastic()) {
            continue;
        }

        for (std::size_t j = 0; j != patterns.size(); ++j) {
            const ::pdbs::Pattern& pattern = patterns[j];

            // Get the syntactically projected operator
            SyntacticProjection syntactic_proj_op = buildSyntacticProjection(pattern, op);

            // If the operator is "truly stochastic" add it to the set
            if (isStochastic(syntactic_proj_op) && !isPseudoDeterministic(syntactic_proj_op)) {
                pattern2proboperators[j].insert(i);
            }
        }
    }

    // There is an edge from pattern i to j if they don't have common operators
    // that are probabilistic when projected
    for (std::size_t i = 0; i != patterns.size(); ++i) {
        const std::set<OperatorID>& prob_operators_i = pattern2proboperators[i];

        for (std::size_t j = i + 1; j != patterns.size(); ++j) {
            const std::set<OperatorID>& prob_operators_j = pattern2proboperators[j];

            if (areDisjoint(prob_operators_i, prob_operators_j)) {
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

}
}
}
