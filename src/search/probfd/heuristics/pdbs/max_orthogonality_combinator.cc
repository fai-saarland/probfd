#include "probfd/heuristics/pdbs/max_orthogonality_combinator.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollections.h"

#include "downward/algorithms/max_cliques.h"

#include "downward/pdbs/pattern_cliques.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

using namespace std::views;

MaxOrthogonalityCombinatorBase::MaxOrthogonalityCombinatorBase(
    ProbabilisticTaskProxy task_proxy)
    : var_orthogonality(compute_prob_orthogonal_vars(task_proxy, false))
{
}

void MaxOrthogonalityCombinatorBase::update(const PPDBCollection& pdbs)
{
    std::vector<std::vector<int>> c_graph =
        build_compatibility_graph_orthogonality(
            pdbs |
                transform([](const auto& pdb) { return pdb->get_pattern(); }),
            var_orthogonality);

    max_cliques::compute_max_cliques(c_graph, subcollections_);
}

std::vector<PatternSubCollection>
MaxOrthogonalityCombinatorBase::compute_subcollections_with_pdbs(
    const PPDBCollection& pdbs,
    const ProbabilityAwarePatternDatabase& new_pdb)
{
    return ::pdbs::compute_pattern_cliques_with_pattern(
        pdbs | transform([](const auto& pdb) { return pdb->get_pattern(); }),
        subcollections_,
        new_pdb.get_pattern(),
        var_orthogonality);
}

value_t AdditiveMaxOrthogonalityCombinator::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    auto result = 0_vt;

    for (int pattern_id : subcollection) {
        result += pdb_estimates[pattern_id];
    }

    return result;
}

value_t
AdditiveMaxOrthogonalityCombinator::combine(value_t left, value_t right) const
{
    return left + right;
}

value_t MultiplicativeMaxOrthogonalityCombinator::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    auto result = 1_vt;

    for (int pattern_id : subcollection) {
        result *= 1 - pdb_estimates[pattern_id];
    }

    return 1 - result;
}

value_t
MultiplicativeMaxOrthogonalityCombinator::combine(value_t left, value_t right)
    const
{
    return 1_vt - (1_vt - left) * (1_vt - right);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd