#include "independence.h"

#include "../../../globals.h"
#include "../syntactic_projection.h"

namespace probabilistic {
namespace pdbs{
namespace multiplicativity {

using namespace syntactic_projection;

using VariableOrthogonality = std::vector<std::vector<bool>>;

namespace {

// Helper class to iterate over permutations of values
template <typename T>
struct Permutation {
    const std::vector<T> begin_values;
    const std::vector<T> end_values;
    std::vector<T> values;

    Permutation(
        std::vector<T> begin_values,
        std::vector<T> end_values,
        std::vector<T> values)
        : begin_values(std::move(begin_values))
        , end_values(std::move(end_values))
        , values(std::move(values))
    {
    }

    ~Permutation() = default;

    bool get_next() {
        for (std::size_t i = 0; i != values.size(); ++i) {
            if (values[i] != end_values[i]) {
                // If this dimension can handle another increment... then done.
                ++values[i];
                return true;
            }

            // Otherwise, reset this dimension and bubble up to the next dimension
            // to take a look
            values[i] = begin_values[i];
        }

        return false;
    }

    T& operator[](int i) {
        return values[i];
    }

    const T& operator[](int i) const {
        return values[i];
    }
};

}

bool is_independent_operator(
    const PatternCollection& patterns,
    const Pattern& union_pattern,
    const ProbabilisticOperator& op)
{
    using ProjectionOutcomeIterator = ProjectionOperator::const_iterator;

    const std::size_t num_patterns = patterns.size();

    // Build the projected operators
    ProjectionOperator abs_op_union = project_operator(union_pattern, op);
    std::vector<ProjectionOperator> abs_op_individual;
    {
        for (std::size_t i = 0; i < num_patterns; ++i) {
            abs_op_individual[i] = project_operator(patterns[i], op);
        }
    }

    // Set up iteration over all possible permutations of outcomes...
    std::vector<ProjectionOutcomeIterator> proj_outcomes_begins(num_patterns);
    std::vector<ProjectionOutcomeIterator> proj_outcomes_ends(num_patterns);
    {
        for (std::size_t i = 0; i != num_patterns; ++i) {
            proj_outcomes_begins[i] = abs_op_individual[i].cbegin();
            proj_outcomes_ends[i] = abs_op_individual[i].cend();
        }
    }

    Permutation<ProjectionOutcomeIterator> proj_outcomes_permutation(
        proj_outcomes_begins, proj_outcomes_ends, proj_outcomes_begins);

    // Check if every permutation of outcomes has the same probability
    // in the union.
    do {
        std::vector<std::pair<int, int>> union_effects;
        value_type::value_t indep_prob = value_type::one;

        for (std::size_t i = 0; i != num_patterns; ++i) {
            const auto& [effects, prob] = *proj_outcomes_permutation[i];

            // NOTE: This assumes patterns are disjoint!
            union_effects.insert(
                union_effects.end(), effects.begin(), effects.end());
            indep_prob *= prob;
        }

        value_type::value_t union_prob =
            abs_op_union.get_probability(union_effects);

        if (!value_type::approx_equal()(indep_prob, union_prob)) {
            return false;
        }
    } while (proj_outcomes_permutation.get_next());

    return true;
}

bool is_independent_collection(const PatternCollection& patterns) {
    // Construct union pattern here
    Pattern union_pattern;
    {
        for (const Pattern &pattern : patterns) {
            for (int var : pattern) {
                auto it = std::lower_bound(
                    union_pattern.begin(), union_pattern.end(), var);

                // Duplicate variable -> not disjoint
                if (*it == var) {
                    return false;
                }

                union_pattern.insert(it, var);
            }
        }
    }

    for (const ProbabilisticOperator& op : g_operators) {
        if (!is_independent_operator(patterns, union_pattern, op)) {
            return false;
        }
    }

    return true;
}

}
}
}
