#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_ABSTRACT_SOLUTION_DATA_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_ABSTRACT_SOLUTION_DATA_H

#include "../../types.h"

#include "../../../../../utils/logging.h"
#include "../../abstract_policy.h"

#include <memory>
#include <ostream>
#include <set>
#include <vector>

namespace utils {
class RandomNumberGenerator;
}

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class AbstractSolutionData {
    std::unique_ptr<PDBType> pdb;
    std::set<int> blacklist;
    AbstractPolicy policy;
    bool solved;

public:
    AbstractSolutionData(const Pattern& pattern, std::set<int> blacklist);

    const Pattern& get_pattern() const;

    void blacklist_variable(int var);

    bool is_blacklisted(int var) const;

    const std::set<int>& get_blacklist() const;

    const PDBType& get_pdb() const;

    std::unique_ptr<PDBType> steal_pdb();

    const AbstractPolicy& get_policy() const;

    value_type::value_t get_policy_cost() const;

    bool is_solved() const;

    void mark_as_solved();

    bool solution_exists() const;
};

using ExpCostAbstractSolutionData = AbstractSolutionData<ExpCostProjection>;
using MaxProbAbstractSolutionData = AbstractSolutionData<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic
#endif
