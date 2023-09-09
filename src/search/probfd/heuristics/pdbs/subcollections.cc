#include "probfd/heuristics/pdbs/subcollections.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include <utility>

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {

class SyntacticProjectionOperator {
    std::map<std::vector<FactPair>, value_t> effects_to_probs;

public:
    using const_iterator = decltype(std::as_const(effects_to_probs).begin());

    SyntacticProjectionOperator(
        const Pattern& pattern,
        const ProbabilisticOperatorProxy& op)
    {
        for (const ProbabilisticOutcomeProxy& outcome : op.get_outcomes()) {
            // Project effects
            std::vector<FactPair> projected_effects;

            for (const ProbabilisticEffectProxy& effect :
                 outcome.get_effects()) {
                const auto& [var, val] = effect.get_fact().get_pair();

                if (utils::contains(pattern, var)) {
                    projected_effects.emplace_back(var, val);
                }
            }

            const auto probability = outcome.get_probability();

            if (auto p = effects_to_probs.emplace(
                    std::move(projected_effects),
                    probability);
                !p.second) {
                p.first->second += probability;
            }
        }
    }

    value_t get_probability(const std::vector<FactPair>& effects) const
    {
        auto it = effects_to_probs.find(effects);
        return it != effects_to_probs.end() ? it->second : 0_vt;
    }

    bool is_stochastic() const { return effects_to_probs.size() > 1; }
    bool is_pseudo_deterministic() const
    {
        if (effects_to_probs.size() == 2) {
            auto it = effects_to_probs.cbegin();
            if (it->first.empty() || (++it)->first.empty()) {
                return true;
            }
        }

        return false;
    }

    auto begin() { return effects_to_probs.begin(); }
    auto end() { return effects_to_probs.end(); }

    auto begin() const { return effects_to_probs.begin(); }
    auto end() const { return effects_to_probs.end(); }
};

template <typename T>
bool are_disjoint(const std::vector<T>& A, const std::vector<T>& B)
{
    std::vector<T> intersection;

    std::set_intersection(
        A.cbegin(),
        A.cend(),
        B.cbegin(),
        B.cend(),
        std::back_inserter(intersection));

    return intersection.empty();
}

} // namespace

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    ProbabilisticTaskProxy task_proxy,
    bool ignore_deterministic)
{
    const size_t num_vars = task_proxy.get_variables().size();

    std::vector<std::vector<bool>> are_orthogonal(
        num_vars,
        std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperatorProxy& op : task_proxy.get_operators()) {
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();

        if (ignore_deterministic && outcomes.size() == 1) {
            continue;
        }

        std::unordered_set<int> affected_vars;

        for (const ProbabilisticOutcomeProxy& outcome : outcomes) {
            for (const auto& effect : outcome.get_effects()) {
                const int new_var = effect.get_fact().get_variable().get_id();
                if (affected_vars.insert(new_var).second) {
                    are_orthogonal[new_var][new_var] = false;
                    for (const int old_var : affected_vars) {
                        if (old_var == new_var) continue;
                        are_orthogonal[old_var][new_var] = false;
                        are_orthogonal[new_var][old_var] = false;
                    }
                }
            }
        }
    }

    return are_orthogonal;
}

std::vector<std::vector<int>> build_compatibility_graph_weak_orthogonality(
    ProbabilisticTaskProxy task_proxy,
    const PatternCollection& patterns)
{
    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    std::vector<std::vector<size_t>> prob_operators;
    prob_operators.resize(patterns.size());

    // Compute operators that are probabilistic when projected for each pattern
    for (const ProbabilisticOperatorProxy op : task_proxy.get_operators()) {
        // A lot of actions are already deterministic in the first place, so
        // we can save some work
        if (op.get_outcomes().size() == 1) {
            continue;
        }

        for (std::size_t j = 0; j != patterns.size(); ++j) {
            const Pattern& pattern = patterns[j];

            // Get the syntactically projected operator
            SyntacticProjectionOperator abs_op(pattern, op);

            // If the operator is "truly stochastic" add it to the set
            if (abs_op.is_stochastic() && !abs_op.is_pseudo_deterministic()) {
                prob_operators[j].push_back(op.get_id());
            }
        }
    }

    // There is an edge from pattern i to j if they don't have common operators
    // that are probabilistic when projected
    for (std::size_t i = 0; i != patterns.size(); ++i) {
        const auto& prob_operators_i = prob_operators[i];

        for (std::size_t j = i + 1; j != patterns.size(); ++j) {
            const auto& prob_operators_j = prob_operators[j];

            if (are_disjoint(prob_operators_i, prob_operators_j)) {
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
