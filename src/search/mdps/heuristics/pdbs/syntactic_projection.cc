#include "syntactic_projection.h"

namespace probabilistic {
namespace heuristics {
namespace pdbs {
namespace syntactic_projection {

value_type::value_t ProjectionOperator::get_probability(
    const std::vector<std::pair<int, int>>& effects) const
{
    auto it = effects_to_probs.find(effects);
    return it != effects_to_probs.end() ? it->second : value_type::zero;
}

void ProjectionOperator::add_effect_probability(
    const std::vector<std::pair<int, int>>& effects,
    value_type::value_t probability)
{
    auto it = effects_to_probs.find(effects);

    if (it != effects_to_probs.end()) {
        it->second += probability;
    } else {
        effects_to_probs[effects] = probability;
    }
}

bool ProjectionOperator::is_stochastic() const
{
    return effects_to_probs.size() > 1;
}

bool ProjectionOperator::is_pseudo_deterministic() const
{
    if (effects_to_probs.size() == 2) {
        auto it = effects_to_probs.cbegin();
        if (it->first.empty() || (++it)->first.empty()) {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<int, int>> project_effects(
    const Pattern& pattern,
    const std::vector<GlobalEffect>& effects)
{
    std::vector<std::pair<int, int>> projected_effects;

    for (const auto& [var, val, _] : effects) {
        const auto it = std::find(pattern.begin(), pattern.end(), var);
        if (it != pattern.end()) {
            projected_effects.emplace_back(var, val);
        }
    }

    return projected_effects;
}

ProjectionOperator
project_operator(const Pattern& pattern, const ProbabilisticOperator& op)
{
    ProjectionOperator abs_operator;

    for (const ProbabilisticOutcome& outcome : op) {
        const auto& effects = outcome.op->get_effects();
        const value_type::value_t probability = outcome.prob;

        std::vector<std::pair<int, int>> abstract_effects =
            project_effects(pattern, effects);

        abs_operator.add_effect_probability(abstract_effects, probability);
    }

    return abs_operator;
}

} // namespace syntactic_projection
} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic
