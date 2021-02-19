#pragma once

#include <vector>
#include <map>

#include "../../../globals.h"
#include "../../../probabilistic_operator.h"
#include "../syntactic_projection.h"
#include "../../../../pdbs/pattern_cliques.h"

namespace probabilistic {
namespace pdbs {
namespace multiplicativity {

using VariableOrthogonality = std::vector<std::vector<bool>>;

namespace {

// Return true if still incrementable, false if max value was reached
template <typename T>
bool incrementOrdinate(
    std::vector<T>& ordinate,
    std::size_t dimensions,
    const std::vector<T>& dim_val_begins,
    const std::vector<T>& dim_val_ends)
{
    // iterate over dimensions in reverse...
    for (int dimension = dimensions - 1; dimension >= 0; dimension--) {
        if (ordinate[dimension] != dim_val_ends[dimension]) {
            // If this dimension can handle another increment... then done.
            ++ordinate[dimension];
            return true;
        }

        // Otherwise, reset this dimension and bubble up to the next dimension to take a look
        ordinate[dimension] = dim_val_begins[dimension];
    }

    return false;
}

}

template <typename PatternIt>
bool isIndependentCollectionForOperator(
    PatternIt pattern_begin,
    PatternIt pattern_end,
    const ::pdbs::Pattern& union_pattern,
    const ProbabilisticOperator& op)
{
    using Pattern = ::pdbs::Pattern;
    using namespace syntactic_projection;

    const std::size_t num_patterns = std::distance(pattern_begin, pattern_end);

    // Build the Syntactic Projections

    SyntacticProjection union_projection = buildSyntacticProjection(union_pattern, op);

    std::vector<SyntacticProjection> syntactic_projections;

    std::size_t i = 0;
    for (PatternIt b = pattern_begin; b != pattern_end; ++b, ++i) {
        const Pattern& pattern = *b;
        syntactic_projections[i] = buildSyntacticProjection(pattern, op);
    }

    // Set up n-dimensional iteration...

    std::vector<SyntacticProjection::const_iterator> proj_begins(num_patterns);
    std::vector<SyntacticProjection::const_iterator> proj_ends(num_patterns);
    std::vector<SyntacticProjection::const_iterator> ordinate(num_patterns);

    for (std::size_t i = 0; i != num_patterns; ++i) {
        proj_begins[i] = syntactic_projections[i].cbegin();
        proj_ends[i] = syntactic_projections[i].cend();
    }

    for (std::size_t i = 0; i != num_patterns; ++i) {
        ordinate[i] = proj_begins[i];
    }

    // Done. Now check if every combination of outcomes has the same probability in the union

    do {
        Outcome union_outcome;
        value_type::value_t indep_prob = value_type::one;

        for (std::size_t i = 0; i != num_patterns; ++i) {
            const auto& outcome_prob_pair = *ordinate[i];

            const auto& outcome = outcome_prob_pair.first;
            const auto& probability = outcome_prob_pair.second;

            union_outcome.insert(union_outcome.end(), outcome.begin(), outcome.end());
            indep_prob *= probability;
        }

        value_type::value_t union_prob = union_projection[union_outcome];

        if (indep_prob != union_prob) {
            return false;
        }

    } while (incrementOrdinate(ordinate, num_patterns, proj_begins, proj_ends));

    return true;
}

template <typename PatternIt>
bool isIndependentCollection(PatternIt pattern_begin, PatternIt pattern_end) {
    static_assert(std::is_convertible<decltype(*pattern_begin), ::pdbs::Pattern&>::value, "Not a pattern iterator!");
    static_assert(std::is_convertible<decltype(*pattern_end), ::pdbs::Pattern&>::value, "Not a pattern iterator!");

    for (const ProbabilisticOperator& op : g_operators) {
        if (!isIndependentCollectionForOperator(pattern_begin, pattern_end, op)) {
            return false;
        }
    }

    return true;
}

template <bool ignore_deterministic>
std::vector<int> get_affected_vars(const ProbabilisticOperator& op) {
    std::vector<int> affected_vars;

    if constexpr (ignore_deterministic) {
        if (!op.is_stochastic()) {
            return affected_vars;
        }
    }

    // Compute the variables that may be changed by the operator
    for (const ProbabilisticOutcome& outcome : op) {
        const auto effects = outcome.op->get_effects();

        for (const ::GlobalEffect& eff : outcome.op->get_effects()) {
            affected_vars.push_back(eff.var);
        }
    }

    return affected_vars;
}

template <bool ignore_deterministic = true>
VariableOrthogonality compute_prob_orthogonal_vars() {
    const std::size_t num_vars = g_variable_domain.size();

    VariableOrthogonality are_orthogonal(num_vars, std::vector<bool>(num_vars, true));

    for (const ProbabilisticOperator& op : g_operators) {
        const std::vector<int> affected_vars =
            get_affected_vars<ignore_deterministic>(op);

        for (unsigned i1 = 0; i1 < affected_vars.size(); i1++) {
            int var1 = affected_vars[i1];
            are_orthogonal[var1][var1] = false;
            for (unsigned i2 = i1 + 1; i2 < affected_vars.size(); i2++) {
                int var2 = affected_vars[i2];
                are_orthogonal[var1][var2] = false;
                are_orthogonal[var2][var1] = false;
            }
        }
    }

    return are_orthogonal;
}

template <bool ignore_deterministic=false>
std::vector<std::vector<int>> buildCompatibilityGraphOrthogonality(
    const ::pdbs::PatternCollection& patterns)
{
    VariableOrthogonality are_orthogonal =
        compute_prob_orthogonal_vars<ignore_deterministic>();

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (::pdbs::are_patterns_additive(patterns[i], patterns[j], are_orthogonal)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

std::vector<std::vector<int>> buildCompatibilityGraphWeakOrthogonality(
    const ::pdbs::PatternCollection& patterns);

}
}
}
