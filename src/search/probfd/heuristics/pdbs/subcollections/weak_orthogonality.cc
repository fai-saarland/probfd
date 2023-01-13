#include "probfd/heuristics/pdbs/subcollections/weak_orthogonality.h"
#include "probfd/heuristics/pdbs/subcollections/syntactic_projection.h"

#include "pdbs/pattern_cliques.h"

namespace probfd {
namespace heuristics {

namespace pdbs {

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

std::vector<std::vector<int>> build_compatibility_graph_weak_orthogonality(
    const ProbabilisticTaskProxy& task_proxy,
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    std::vector<std::vector<size_t>> prob_operators;
    prob_operators.resize(patterns.size());

    // Compute operators that are probabilistic when projected for each pattern
    for (const ProbabilisticOperatorProxy op : task_proxy.get_operators()) {
        // A lot of actions are already deterministic in the first place, so
        // we can save some work
        if (op.get_outcomes().size() == 1) {
            continue;
        }

        for (std::size_t j = 0; j != patterns.size(); ++j) {
            const Pattern& pattern = patterns[j];

            // Get the syntactically projected operator
            const auto& abs_op = project_operator(pattern, op);

            // If the operator is "truly stochastic" add it to the set
            if (abs_op.is_stochastic() && !abs_op.is_pseudo_deterministic()) {
                prob_operators[j].push_back(op.get_id());
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

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
