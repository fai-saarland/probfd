#ifndef PDBS_DOMINANCE_PRUNING_H
#define PDBS_DOMINANCE_PRUNING_H

#include "pdbs/pattern_database.h"
#include "pdbs/types.h"

#include "utils/countdown_timer.h"
#include "utils/hash.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace pdbs {

class Pruner {
    /*
      Algorithm for pruning dominated patterns.

      "patterns" is the vector of patterns used.
      Each pattern is a vector of variable IDs.

      "pattern_cliques" is the vector of pattern cliques.

      The algorithm works by setting a "current pattern collection"
      against which other patterns and collections can be tested for
      dominance efficiently.

      "variable_to_pattern_id" encodes the relevant information about the
      current clique. For every variable v, variable_to_pattern_id[v] is
      the id of the pattern containing v in the current clique,
      or -1 if the variable is not contained in the current
      clique. (Note that patterns in a pattern clique must be
      disjoint, which is verified by an assertion in debug mode.)

      To test if a given pattern v_1, ..., v_k is dominated by the
      current clique, we check that all entries variable_to_pattern_id[v_i]
      are equal and different from -1.

      "dominated_patterns" is a vector<bool> that can be used to
      quickly test whether a given pattern is dominated by the current
      clique. This is precomputed for every pattern whenever the
      current clique is set.
    */

    const PatternCollection &patterns;
    const std::vector<PatternClique> &pattern_cliques;
    const int num_variables;

    std::vector<int> variable_to_pattern_id;
    std::vector<bool> dominated_patterns;

    void set_current_clique(int clique_id);

    bool is_pattern_dominated(int pattern_id) const;

    bool is_clique_dominated(int clique_id) const;

public:
    Pruner(
        const PatternCollection &patterns,
        const std::vector<PatternClique> &pattern_cliques,
        int num_variables);

    std::vector<bool> get_pruned_cliques(const utils::CountdownTimer &timer);
};


/*
  Clique superset dominates clique subset iff for every pattern
  p_subset in subset there is a pattern p_superset in superset where
  p_superset is a superset of p_subset.
*/
template<typename T>
void prune_dominated_cliques(
    PatternCollection &patterns,
    std::vector<T> &pdbs,
    std::vector<PatternClique> &pattern_cliques,
    int num_variables,
    double max_time) {
    std::cout << "Running dominance pruning..." << std::endl;
    utils::CountdownTimer timer(max_time);

    int num_patterns = patterns.size();
    int num_cliques = pattern_cliques.size();

    std::vector<bool> pruned = Pruner(
        patterns,
        pattern_cliques,
        num_variables).get_pruned_cliques(timer);

    std::vector<PatternClique> remaining_pattern_cliques;
    std::vector<bool> is_remaining_pattern(num_patterns, false);
    int num_remaining_patterns = 0;
    for (size_t i = 0; i < pattern_cliques.size(); ++i) {
        if (!pruned[i]) {
            PatternClique &clique = pattern_cliques[i];
            for (PatternID pattern_id : clique) {
                if (!is_remaining_pattern[pattern_id]) {
                    is_remaining_pattern[pattern_id] = true;
                    ++num_remaining_patterns;
                }
            }
            remaining_pattern_cliques.push_back(move(clique));
        }
    }

    PatternCollection remaining_patterns;
    std::vector<T> remaining_pdbs;
    remaining_patterns.reserve(num_remaining_patterns);
    remaining_pdbs.reserve(num_remaining_patterns);
    std::vector<PatternID> old_to_new_pattern_id(num_patterns, -1);
    for (PatternID old_pattern_id = 0;
         old_pattern_id < num_patterns; ++old_pattern_id) {
        if (is_remaining_pattern[old_pattern_id]) {
            PatternID new_pattern_id = remaining_patterns.size();
            old_to_new_pattern_id[old_pattern_id] = new_pattern_id;
            remaining_patterns.push_back(std::move(patterns[old_pattern_id]));
            remaining_pdbs.push_back(std::move(pdbs[old_pattern_id]));
        }
    }
    for (PatternClique &clique : remaining_pattern_cliques) {
        for (size_t i = 0; i < clique.size(); ++i) {
            PatternID old_pattern_id = clique[i];
            PatternID new_pattern_id = old_to_new_pattern_id[old_pattern_id];
            assert(new_pattern_id != -1);
            clique[i] = new_pattern_id;
        }
    }

    int num_pruned_collections = num_cliques - remaining_pattern_cliques.size();
    std::cout << "Pruned " << num_pruned_collections << " of " << num_cliques
              << " pattern cliques" << std::endl;

    int num_pruned_patterns = num_patterns - num_remaining_patterns;
    std::cout << "Pruned " << num_pruned_patterns << " of " << num_patterns
              << " PDBs" << std::endl;

    patterns.swap(remaining_patterns);
    pdbs.swap(remaining_pdbs);
    pattern_cliques.swap(remaining_pattern_cliques);

    std::cout << "Dominance pruning took " << timer.get_elapsed_time()
              << std::endl;
}
}

#endif
