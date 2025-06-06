#include "downward/pdbs/pattern_collection_generator_systematic.h"

#include "downward/pdbs/validation.h"

#include "downward/task_proxy.h"

#include "downward/task_utils/causal_graph.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

namespace downward::pdbs {
static bool
patterns_are_disjoint(const Pattern& pattern1, const Pattern& pattern2)
{
    size_t i = 0;
    size_t j = 0;
    for (;;) {
        if (i == pattern1.size() || j == pattern2.size()) return true;
        int val1 = pattern1[i];
        int val2 = pattern2[j];
        if (val1 == val2)
            return false;
        else if (val1 < val2)
            ++i;
        else
            ++j;
    }
}

static void compute_union_pattern(
    const Pattern& pattern1,
    const Pattern& pattern2,
    Pattern& result)
{
    result.clear();
    result.reserve(pattern1.size() + pattern2.size());
    set_union(
        pattern1.begin(),
        pattern1.end(),
        pattern2.begin(),
        pattern2.end(),
        back_inserter(result));
}

PatternCollectionGeneratorSystematic::PatternCollectionGeneratorSystematic(
    int pattern_max_size,
    bool only_interesting_patterns,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_pattern_size(pattern_max_size)
    , only_interesting_patterns(only_interesting_patterns)
{
}

void PatternCollectionGeneratorSystematic::compute_eff_pre_neighbors(
    const causal_graph::CausalGraph& cg,
    const Pattern& pattern,
    vector<int>& result) const
{
    /*
      Compute all variables that are reachable from pattern by an
      (eff, pre) arc and are not already contained in the pattern.
    */
    unordered_set<int> candidates;

    // Compute neighbors.
    for (int var : pattern) {
        const vector<int>& neighbors = cg.get_eff_to_pre(var);
        candidates.insert(neighbors.begin(), neighbors.end());
    }

    // Remove elements of pattern.
    for (int var : pattern) {
        candidates.erase(var);
    }

    result.assign(candidates.begin(), candidates.end());
}

void PatternCollectionGeneratorSystematic::compute_connection_points(
    const causal_graph::CausalGraph& cg,
    const Pattern& pattern,
    vector<int>& result) const
{
    /*
      The "connection points" of a pattern are those variables of which
      one must be contained in an SGA pattern that can be attached to this
      pattern to form a larger interesting pattern. (Interesting patterns
      are disjoint unions of SGA patterns.)

      A variable is a connection point if it satisfies the following criteria:
      1. We can get from the pattern to the connection point via
         a (pre, eff) or (eff, eff) arc in the causal graph.
      2. It is not part of pattern.
      3. We *cannot* get from the pattern to the connection point
         via an (eff, pre) arc.

      Condition 1. is the important one. The other conditions are
      optimizations that help reduce the number of candidates to
      consider.
    */
    unordered_set<int> candidates;

    // Handle rule 1.
    for (int var : pattern) {
        const vector<int>& succ = cg.get_successors(var);
        candidates.insert(succ.begin(), succ.end());
    }

    // Handle rules 2 and 3.
    for (int var : pattern) {
        // Rule 2:
        candidates.erase(var);
        // Rule 3:
        const vector<int>& eff_pre = cg.get_eff_to_pre(var);
        for (int pre_var : eff_pre) candidates.erase(pre_var);
    }

    result.assign(candidates.begin(), candidates.end());
}

void PatternCollectionGeneratorSystematic::enqueue_pattern_if_new(
    const Pattern& pattern)
{
    if (pattern_set.insert(pattern).second) patterns->push_back(pattern);
}

void PatternCollectionGeneratorSystematic::build_sga_patterns(
    const TaskProxy& task_proxy,
    const causal_graph::CausalGraph& cg)
{
    assert(max_pattern_size >= 1);
    assert(pattern_set.empty());
    assert(patterns && patterns->empty());

    /*
      SGA patterns are "single-goal ancestor" patterns, i.e., those
      patterns which can be generated by following eff/pre arcs from a
      single goal variable.

      This method must generate all SGA patterns up to size
      "max_pattern_size". They must be generated in order of
      increasing size, and they must be placed in "patterns".

      The overall structure of this is a similar processing queue as
      in the main pattern generation method below, and we reuse
      "patterns" and "pattern_set" between the two methods.
    */

    // Build goal patterns.
    for (FactProxy goal : task_proxy.get_goals()) {
        int var_id = goal.get_variable().get_id();
        Pattern goal_pattern;
        goal_pattern.push_back(var_id);
        enqueue_pattern_if_new(goal_pattern);
    }

    /*
      Grow SGA patterns untill all patterns are processed. Note that
      the patterns vectors grows during the computation.
    */
    for (size_t pattern_no = 0; pattern_no < patterns->size(); ++pattern_no) {
        // We must copy the pattern because references to patterns can be
        // invalidated.
        Pattern pattern = (*patterns)[pattern_no];
        if (pattern.size() == max_pattern_size) break;

        vector<int> neighbors;
        compute_eff_pre_neighbors(cg, pattern, neighbors);

        for (int neighbor_var_id : neighbors) {
            Pattern new_pattern(pattern);
            new_pattern.push_back(neighbor_var_id);
            sort(new_pattern.begin(), new_pattern.end());

            enqueue_pattern_if_new(new_pattern);
        }
    }

    pattern_set.clear();
}

void PatternCollectionGeneratorSystematic::build_patterns(
    const TaskProxy& task_proxy)
{
    int num_variables = task_proxy.get_variables().size();
    const causal_graph::CausalGraph& cg = task_proxy.get_causal_graph();

    // Generate SGA (single-goal-ancestor) patterns.
    // They are generated into the patterns variable,
    // so we swap them from there.
    build_sga_patterns(task_proxy, cg);
    PatternCollection sga_patterns;
    patterns->swap(sga_patterns);

    /* Index the SGA patterns by variable.

       Important: sga_patterns_by_var[var] must be sorted by size.
       This is guaranteed because build_sga_patterns generates
       patterns ordered by size.
    */
    vector<vector<const Pattern*>> sga_patterns_by_var(num_variables);
    for (const Pattern& pattern : sga_patterns) {
        for (int var : pattern) {
            sga_patterns_by_var[var].push_back(&pattern);
        }
    }

    // Enqueue the SGA patterns.
    for (const Pattern& pattern : sga_patterns) enqueue_pattern_if_new(pattern);

    if (log.is_at_least_normal()) {
        log << "Found " << sga_patterns.size() << " SGA patterns." << endl;
    }

    /*
      Combine patterns in the queue with SGA patterns until all
      patterns are processed. Note that the patterns vectors grows
      during the computation.
    */
    for (size_t pattern_no = 0; pattern_no < patterns->size(); ++pattern_no) {
        // We must copy the pattern because references to patterns can be
        // invalidated.
        Pattern pattern1 = (*patterns)[pattern_no];

        vector<int> neighbors;
        compute_connection_points(cg, pattern1, neighbors);

        for (int neighbor_var : neighbors) {
            const auto& candidates = sga_patterns_by_var[neighbor_var];
            for (const Pattern* p_pattern2 : candidates) {
                const Pattern& pattern2 = *p_pattern2;
                if (pattern1.size() + pattern2.size() > max_pattern_size)
                    break; // All remaining candidates are too large.
                if (patterns_are_disjoint(pattern1, pattern2)) {
                    Pattern new_pattern;
                    compute_union_pattern(pattern1, pattern2, new_pattern);
                    enqueue_pattern_if_new(new_pattern);
                }
            }
        }
    }

    pattern_set.clear();
    if (log.is_at_least_normal()) {
        log << "Found " << patterns->size() << " interesting patterns." << endl;
    }
}

void PatternCollectionGeneratorSystematic::build_patterns_naive(
    const TaskProxy& task_proxy)
{
    int num_variables = task_proxy.get_variables().size();
    PatternCollection current_patterns(1);
    PatternCollection next_patterns;
    for (size_t i = 0; i < max_pattern_size; ++i) {
        for (const Pattern& current_pattern : current_patterns) {
            int max_var = -1;
            if (i > 0) max_var = current_pattern.back();
            for (int var = max_var + 1; var < num_variables; ++var) {
                Pattern pattern = current_pattern;
                pattern.push_back(var);
                next_patterns.push_back(pattern);
                patterns->push_back(pattern);
            }
        }
        next_patterns.swap(current_patterns);
        next_patterns.clear();
    }

    if (log.is_at_least_normal()) {
        log << "Found " << patterns->size() << " patterns." << endl;
    }
}

string PatternCollectionGeneratorSystematic::name() const
{
    return "systematic pattern collection generator";
}

PatternCollectionInformation
PatternCollectionGeneratorSystematic::compute_patterns(
    const shared_ptr<AbstractTask>& task)
{
    TaskProxy task_proxy(*task);
    patterns = make_shared<PatternCollection>();
    pattern_set.clear();
    if (only_interesting_patterns) {
        build_patterns(task_proxy);
    } else {
        build_patterns_naive(task_proxy);
    }
    return PatternCollectionInformation(task_proxy, patterns, log);
}

} // namespace pdbs
