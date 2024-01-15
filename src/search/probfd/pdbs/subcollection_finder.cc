#include "probfd/pdbs/subcollection_finder.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include <algorithm>
#include <cstddef>
#include <numeric>

namespace probfd {
namespace pdbs {

value_t SubCollectionFinder::evaluate(
    const PPDBCollection& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state,
    value_t termination_cost)
{
    if (database.empty()) return 0_vt;

    // Get pattern estimates
    std::vector<value_t> estimates(database.size());
    for (std::size_t i = 0; i != database.size(); ++i) {
        const value_t estimate = database[i]->lookup_estimate(state);

        if (estimate == termination_cost) {
            return estimate;
        }

        estimates[i] = estimate;
    }

    // Get lowest additive subcollection value
    auto transformer = [&, this](const std::vector<int>& subcollection) {
        return this->evaluate_subcollection(estimates, subcollection);
    };

    return std::transform_reduce(
        subcollections.begin(),
        subcollections.end(),
        0_vt,
        static_cast<const value_t& (*)(const value_t&, const value_t&)>(
            std::max<value_t>),
        transformer);
}

} // namespace pdbs
} // namespace probfd