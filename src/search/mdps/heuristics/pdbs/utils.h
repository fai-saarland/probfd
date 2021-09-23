#ifndef MDPS_HEURISTICS_PDBS_UTILS_H
#define MDPS_HEURISTICS_PDBS_UTILS_H

#include "../../evaluation_result.h"
#include "../../value_type.h"
#include "types.h"

#include <algorithm>
#include <numeric>
#include <ostream>

class GlobalState;

namespace probabilistic {
namespace pdbs {

template <class PDBType>
value_type::value_t evaluate_subcollection(
    const std::vector<value_type::value_t>& pdb_estimates,
    const std::vector<int>& subcollection)
{
    constexpr bool is_expcost = std::is_same_v<PDBType, ExpCostProjection>;

    auto transformer = [&pdb_estimates](int i) { return pdb_estimates[i]; };

    auto neutral_element = is_expcost ? value_type::zero : value_type::one;

    using R = std::conditional_t<
        is_expcost,
        std::plus<value_type::value_t>,
        std::multiplies<value_type::value_t>>;

    return std::transform_reduce(
        subcollection.begin(),
        subcollection.end(),
        neutral_element,
        R(),
        transformer);
}

template <class PDBType>
value_type::value_t combine(value_type::value_t left, value_type::value_t right)
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
    const GlobalState& state)
{
    value_type::value_t result = std::is_same_v<PDBType, ExpCostProjection>
                                     ? value_type::zero
                                     : value_type::one;

    if (!database.empty()) {
        // Get pattern estimates
        std::vector<value_type::value_t> estimates(database.size());
        for (std::size_t i = 0; i != database.size(); ++i) {
            auto eval_result = database[i]->evaluate(state);

            if (eval_result) {
                return eval_result;
            }

            estimates[i] = static_cast<value_type::value_t>(eval_result);
        }

        // Get lowest additive subcollection value
        auto transformer = [&estimates](const std::vector<int>& subcollection) {
            return evaluate_subcollection<PDBType>(estimates, subcollection);
        };

        result = std::transform_reduce(
            subcollections.begin(),
            subcollections.end(),
            result,
            static_cast<
                const value_type::
                    value_t& (*)(const value_type::value_t&, const value_type::value_t&)>(
                std::min<value_type::value_t>),
            transformer);
    }

    return {false, result};
}

void dump_pattern_vars(std::ostream& out, const Pattern& p);
void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p);
void dump_pattern(std::ostream& out, PatternID i, const Pattern& p);

} // namespace pdbs
} // namespace probabilistic
#endif // __UTILS_H__