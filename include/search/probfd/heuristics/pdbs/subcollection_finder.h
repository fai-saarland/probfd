#ifndef PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_FINDER_H
#define PROBFD_HEURISTICS_PDBS_SUBCOLLECTION_FINDER_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinder {
public:
    virtual ~SubCollectionFinder() = default;

    virtual std::shared_ptr<std::vector<PatternSubCollection>>
    compute_subcollections(const PatternCollection&) = 0;

    virtual std::vector<PatternSubCollection>
    compute_subcollections_with_pattern(
        const PatternCollection& patterns,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const Pattern& new_pattern) = 0;

    virtual value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const = 0;

    virtual value_t combine(value_t left, value_t right) const = 0;

    value_t evaluate(
        const PPDBCollection& database,
        const std::vector<PatternSubCollection>& subcollections,
        const State& state,
        value_t termination_cost);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __SUBCOLLECTION_FINDER_H__