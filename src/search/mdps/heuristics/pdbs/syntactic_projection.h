#pragma once

#include <map>
#include <vector>

#include "../../globals.h"
#include "../../../global_operator.h"
#include "types.h"


namespace probabilistic {
namespace pdbs {
namespace syntactic_projection {

class ProjectionOperator {
    std::map<std::vector<std::pair<int, int>>, value_type::value_t>
        effects_to_probs;

public:
    using iterator = decltype(effects_to_probs)::iterator;
    using const_iterator = decltype(effects_to_probs)::const_iterator;

    ProjectionOperator() = default;

    value_type::value_t get_probability(
        const std::vector<std::pair<int, int>>& effects) const;

    void add_effect_probability(
        const std::vector<std::pair<int, int>>& effects,
        value_type::value_t probability);

    iterator begin() {
        return effects_to_probs.begin();
    }

    iterator end() {
        return effects_to_probs.end();
    }

    const_iterator cbegin() const {
        return effects_to_probs.cbegin();
    }

    const_iterator cend() const {
        return effects_to_probs.cend();
    }

    bool is_stochastic() const;
    bool is_pseudo_deterministic() const;
};

std::vector<std::pair<int, int>> project_effects(
    const Pattern& pattern,
    const std::vector<GlobalEffect>& effects);

ProjectionOperator project_operator(
    const Pattern& pattern,
    const ProbabilisticOperator& op);

}
}
}
