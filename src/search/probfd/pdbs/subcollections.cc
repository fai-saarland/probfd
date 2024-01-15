#include "probfd/pdbs/subcollections.h"

#include "downward/pdbs/pattern_cliques.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <unordered_set>
#include <utility>

namespace probfd {
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

std::vector<std::vector<int>> build_compatibility_graph_weak_orthogonality(
    const ProbabilisticTaskProxy& task_proxy,
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

bool is_independent_collection(
    const ProbabilisticTaskProxy& task_proxy,
    const PatternCollection& patterns)
{
    using ProjectionOutcomeIterator =
        SyntacticProjectionOperator::const_iterator;

    // Construct union pattern here
    Pattern union_pattern;

    for (const Pattern& pattern : patterns) {
        for (int var : pattern) {
            auto it = std::lower_bound(
                union_pattern.begin(),
                union_pattern.end(),
                var);

            // Duplicate variable -> not disjoint
            if (*it == var) {
                return false;
            }

            union_pattern.insert(it, var);
        }
    }

    const std::size_t num_patterns = patterns.size();

    std::vector<SyntacticProjectionOperator> abs_op_individual;
    std::vector<ProjectionOutcomeIterator> proj_outcomes_values;
    std::vector<std::pair<ProjectionOutcomeIterator, ProjectionOutcomeIterator>>
        proj_outcomes_ranges;

    std::vector<FactPair> union_effects;

    abs_op_individual.reserve(num_patterns);
    proj_outcomes_values.reserve(num_patterns);
    proj_outcomes_ranges.reserve(num_patterns);

    for (const ProbabilisticOperatorProxy& op : task_proxy.get_operators()) {
        // Build the operator of the task projection wrt the pattern union
        SyntacticProjectionOperator abs_op_union(union_pattern, op);

        // Build the operators of the individual task projections
        for (const Pattern& pattern : patterns) {
            const auto& abs_op = abs_op_individual.emplace_back(pattern, op);
            proj_outcomes_values.emplace_back(abs_op.begin());
            proj_outcomes_ranges.emplace_back(abs_op.begin(), abs_op.end());
        }

        // Check if every permutation of outcomes has the same probability
        // in the union.
        Permutation<ProjectionOutcomeIterator> proj_outcomes_permutation(
            proj_outcomes_ranges,
            proj_outcomes_values);

        do {
            value_t indep_prob = 1_vt;

            for (std::size_t i = 0; i != num_patterns; ++i) {
                const auto& [effects, prob] = *proj_outcomes_permutation[i];

                // NOTE: This assumes patterns are disjoint!
                union_effects.insert(
                    union_effects.end(),
                    effects.begin(),
                    effects.end());
                indep_prob *= prob;
            }

            value_t union_prob = abs_op_union.get_probability(union_effects);

            union_effects.clear();

            if (!is_approx_equal(indep_prob, union_prob)) {
                return false;
            }
        } while (proj_outcomes_permutation.get_next());

        abs_op_individual.clear();
        proj_outcomes_values.clear();
        proj_outcomes_ranges.clear();
    }

    return true;
}

} // namespace pdbs
} // namespace probfd
