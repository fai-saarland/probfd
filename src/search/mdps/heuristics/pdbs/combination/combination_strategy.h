#pragma once

#include "../../../evaluation_result.h"
#include "../../../value_type.h"
#include "../types.h"

#include <vector>

class GlobalState;

namespace probabilistic {
namespace pdbs {

class CombinationStrategy {
public:
    virtual ~CombinationStrategy() = default;

    virtual value_type::value_t combine(
        const std::vector<value_type::value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const = 0;

    virtual value_type::value_t
    combine(value_type::value_t left, value_type::value_t right) const = 0;

    template <class PDBType>
    EvaluationResult evaluate(
        const PPDBCollection<PDBType>& database,
        const std::vector<PatternSubCollection>& subcollections_,
        const GlobalState& state) const;
};

} // namespace pdbs
} // namespace probabilistic
