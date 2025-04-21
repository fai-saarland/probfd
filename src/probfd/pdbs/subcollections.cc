#include "probfd/pdbs/subcollections.h"

#include "downward/pdbs/pattern_cliques.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <unordered_set>
#include <utility>

using namespace downward;

namespace probfd::pdbs {

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
                const auto& [var, val] = effect.get_fact();

                if (std::ranges::contains(pattern, var)) {
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

    [[nodiscard]]
    value_t get_probability(const std::vector<FactPair>& effects) const
    {
        auto it = effects_to_probs.find(effects);
        return it != effects_to_probs.end() ? it->second : 0_vt;
    }

    [[nodiscard]]
    bool is_stochastic() const
    {
        return effects_to_probs.size() > 1;
    }
    [[nodiscard]]
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

    [[nodiscard]]
    auto begin() const
    {
        return effects_to_probs.begin();
    }
    [[nodiscard]]
    auto end() const
    {
        return effects_to_probs.end();
    }
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

// Helper class to iterate over permutations of values
template <typename T>
struct Permutation {
    const std::vector<std::pair<T, T>> ranges;
    std::vector<T> values;

    Permutation(std::vector<std::pair<T, T>> ranges, std::vector<T> values)
        : ranges(std::move(ranges))
        , values(std::move(values))
    {
    }

    bool get_next()
    {
        for (std::size_t i = 0; i != values.size(); ++i) {
            if (values[i] != ranges[i].second) {
                // If this dimension can handle another increment... then done.
                ++values[i];
                return true;
            }

            // Otherwise, reset this dimension and bubble up to the next
            // dimension to take a look
            values[i] = ranges[i].first;
        }

        return false;
    }

    T& operator[](int i) { return values[i]; }
    const T& operator[](int i) const { return values[i]; }
};

} // namespace

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    const ProbabilisticTaskProxy& task_proxy,
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
                const int new_var = effect.get_fact().var;
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

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const ProbabilisticTaskProxy& task_proxy,
    const PatternCollection& patterns,
    bool ignore_deterministic)
{
    return build_compatibility_graph_orthogonality(
        patterns,
        compute_prob_orthogonal_vars(task_proxy, ignore_deterministic));
}

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const std::vector<std::vector<bool>>& on_vars)
{
    using ::pdbs::are_patterns_additive;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (are_patterns_additive(patterns[i], patterns[j], on_vars)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

} // namespace probfd::pdbs
