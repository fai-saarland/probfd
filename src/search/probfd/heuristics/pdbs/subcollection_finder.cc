#include "probfd/heuristics/pdbs/subcollection_finder.h"

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "plugin.h"

#include <numeric>

namespace probfd {
namespace heuristics {
namespace pdbs {

EvaluationResult SubCollectionFinder::evaluate(
    const PPDBCollection& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state)
{
    value_t result = 0_vt;

    if (!database.empty()) {
        // Get pattern estimates
        std::vector<value_t> estimates(database.size());
        for (std::size_t i = 0; i != database.size(); ++i) {
            const EvaluationResult eval_result = database[i]->evaluate(state);

            if (eval_result.is_unsolvable()) {
                return eval_result;
            }

            estimates[i] = eval_result.get_estimate();
        }

        // Get lowest additive subcollection value
        auto transformer = [this,
                            &estimates](const std::vector<int>& subcollection) {
            return this->evaluate_subcollection(estimates, subcollection);
        };

        result = std::transform_reduce(
            subcollections.begin(),
            subcollections.end(),
            result,
            static_cast<const value_t& (*)(const value_t&, const value_t&)>(
                std::max<value_t>),
            transformer);
    }

    return {false, result};
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd