#ifndef PROBFD_HEURISTICS_PDBS_UTILS_H
#define PROBFD_HEURISTICS_PDBS_UTILS_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/value_type.h"

#include <algorithm>
#include <numeric>
#include <ostream>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

template <class PDBType>
value_t evaluate_subcollection(
    const std::vector<value_t>& pdb_estimates,
    const std::vector<int>& subcollection)
{
    constexpr bool is_expcost = std::is_same_v<PDBType, ExpCostProjection>;

    auto transformer = [&pdb_estimates](int i) { return pdb_estimates[i]; };

    auto neutral_element = is_expcost ? 0_vt : 1_vt;

    using R = std::
        conditional_t<is_expcost, std::plus<value_t>, std::multiplies<value_t>>;

    return std::transform_reduce(
        subcollection.begin(),
        subcollection.end(),
        neutral_element,
        R(),
        transformer);
}

template <class PDBType>
value_t combine(value_t left, value_t right)
{
    if constexpr (std::is_same_v<PDBType, ExpCostProjection>) {
        return left + right;
    } else {
        return left * right;
    }
}

template <class PDBType>
EvaluationResult evaluate(
    const PPDBCollection<PDBType>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const State& state)
{
    value_t result = std::is_same_v<PDBType, ExpCostProjection> ? 0_vt : 1_vt;

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
        auto transformer = [&estimates](const std::vector<int>& subcollection) {
            return evaluate_subcollection<PDBType>(estimates, subcollection);
        };

        result = std::transform_reduce(
            subcollections.begin(),
            subcollections.end(),
            result,
            static_cast<const value_t& (*)(const value_t&, const value_t&)>(
                std::min<value_t>),
            transformer);
    }

    return {false, result};
}

void dump_pattern_vars(std::ostream& out, const Pattern& p);
void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __UTILS_H__