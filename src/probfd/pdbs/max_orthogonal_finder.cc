#include "probfd/pdbs/max_orthogonal_finder.h"

#include "probfd/pdbs/subcollections.h"

#include "downward/algorithms/max_cliques.h"

#include "downward/pdbs/pattern_cliques.h"

using namespace downward;

namespace probfd::pdbs {

MaxOrthogonalityFinderBase::MaxOrthogonalityFinderBase(
    const ProbabilisticTaskProxy& task_proxy)
    : var_orthogonality_(compute_prob_orthogonal_vars(task_proxy, false))
{
}

std::vector<PatternSubCollection>
MaxOrthogonalityFinderBase::compute_subcollections(
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> c_graph =
        build_compatibility_graph_orthogonality(patterns, var_orthogonality_);

    std::vector<PatternSubCollection> additive_subcollections;
    max_cliques::compute_max_cliques(c_graph, additive_subcollections);

    return additive_subcollections;
}

std::vector<PatternSubCollection>
MaxOrthogonalityFinderBase::compute_subcollections_with_pattern(
    const PatternCollection& patterns,
    const std::vector<PatternSubCollection>& known_pattern_cliques,
    const Pattern& new_pattern)
{
    return ::pdbs::compute_pattern_cliques_with_pattern(
        patterns,
        known_pattern_cliques,
        new_pattern,
        var_orthogonality_);
}

value_t AdditiveMaxOrthogonalityFinder::evaluate_subcollection(
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
AdditiveMaxOrthogonalityFinder::combine(value_t left, value_t right) const
{
    return left + right;
}

value_t MultiplicativeMaxOrthogonalityFinder::evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection) const
{
    auto result = -1_vt;

    for (int pattern_id : subcollection) {
        result *= -pdb_estimates[pattern_id];
    }

    return result;
}

value_t
MultiplicativeMaxOrthogonalityFinder::combine(value_t left, value_t right) const
{
    return -(left * right);
}

} // namespace probfd::pdbs
