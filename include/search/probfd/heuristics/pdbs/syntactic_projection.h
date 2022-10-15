#ifndef MDPS_HEURISTICS_PDBS_SYNTACTIC_PROJECTION_H
#define MDPS_HEURISTICS_PDBS_SYNTACTIC_PROJECTION_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/globals.h"

#include "global_operator.h"

#include <map>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace syntactic_projection {

class ProjectionOperator {
    std::map<std::vector<std::pair<int, int>>, value_type::value_t>
        effects_to_probs;

public:
    using iterator = decltype(effects_to_probs)::iterator;
    using const_iterator = decltype(effects_to_probs)::const_iterator;

    ProjectionOperator() = default;

    value_type::value_t
    get_probability(const std::vector<std::pair<int, int>>& effects) const;

    void add_effect_probability(
        const std::vector<std::pair<int, int>>& effects,
        value_type::value_t probability);

    iterator begin() { return effects_to_probs.begin(); }

    iterator end() { return effects_to_probs.end(); }

    const_iterator cbegin() const { return effects_to_probs.cbegin(); }

    const_iterator cend() const { return effects_to_probs.cend(); }

    bool is_stochastic() const;
    bool is_pseudo_deterministic() const;
};

std::vector<std::pair<int, int>> project_effects(
    const Pattern& pattern,
    const std::vector<GlobalEffect>& effects);

ProjectionOperator
project_operator(const Pattern& pattern, const ProbabilisticOperator& op);

} // namespace syntactic_projection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __SYNTACTIC_PROJECTION_H__