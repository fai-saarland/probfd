#include "independence.h"

#include "../../../globals.h"
#include "../syntactic_projection.h"

namespace probabilistic {
namespace pdbs{
namespace multiplicativity {

using namespace syntactic_projection;

using VariableOrthogonality = std::vector<std::vector<bool>>;

namespace {

// Return true if still incrementable, false if max value was reached
template <typename T>
bool increment_ordinate(
    std::vector<T>& ordinate,
    std::size_t dimensions,
    const std::vector<T>& dim_val_begins,
    const std::vector<T>& dim_val_ends)
{
    // iterate over dimensions in reverse...
    for (int dimension = (int) dimensions - 1; dimension >= 0; dimension--) {
        if (ordinate[dimension] != dim_val_ends[dimension]) {
            // If this dimension can handle another increment... then done.
            ++ordinate[dimension];
            return true;
        }

        // Otherwise, reset this dimension and bubble up to the next dimension
        // to take a look
        ordinate[dimension] = dim_val_begins[dimension];
    }

    return false;
}

}

bool is_independent_operator(
    const PatternCollection& patterns,
    const Pattern& union_pattern,
    const ProbabilisticOperator& op)
{
    using namespace syntactic_projection;

    const std::size_t num_patterns = patterns.size();

    // Build the projected operators
    ProjectionOperator abs_op_union = project_operator(union_pattern, op);
    std::vector<ProjectionOperator> abs_op_individual;
    {
        for (std::size_t i = 0; i < num_patterns; ++i) {
            abs_op_individual[i] = project_operator(patterns[i], op);
        }
    }

    // Set up n-dimensional iteration...
    std::vector<ProjectionOperator::const_iterator> proj_begins(num_patterns);
    std::vector<ProjectionOperator::const_iterator> proj_ends(num_patterns);

    for (std::size_t i = 0; i != num_patterns; ++i) {
        proj_begins[i] = abs_op_individual[i].cbegin();
        proj_ends[i] = abs_op_individual[i].cend();
    }

    std::vector<ProjectionOperator::const_iterator> ordinate = proj_begins;

    // Done. Now check if every combination of outcomes has the same probability
    // in the union.
    do {
        std::vector<std::pair<int, int>> union_outcome;
        value_type::value_t indep_prob = value_type::one;

        for (std::size_t i = 0; i != num_patterns; ++i) {
            const auto& [effects, prob] = *ordinate[i];

            // NOTE: This assumes patterns are disjoint!
            union_outcome.insert(
                union_outcome.end(), effects.begin(), effects.end());
            indep_prob *= prob;
        }

        value_type::value_t union_prob =
            abs_op_union.get_probability(union_outcome);

        if (!value_type::approx_equal()(indep_prob, union_prob)) {
            return false;
        }
    } while (increment_ordinate(ordinate, num_patterns, proj_begins, proj_ends));

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
