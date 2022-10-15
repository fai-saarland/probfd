#include "pdbs/dominance_pruning.h"

#include "pdbs/pattern_database.h"

#include <cassert>
#include <vector>

using namespace std;

namespace pdbs {

void Pruner::set_current_clique(int clique_id) {
    /*
      Set the current pattern collection to be used for
      is_pattern_dominated() or is_collection_dominated(). Compute
      dominated_patterns based on the current pattern collection.
    */
    variable_to_pattern_id.assign(num_variables, -1);
    assert(variable_to_pattern_id == vector<int>(num_variables, -1));
    for (PatternID pattern_id : pattern_cliques[clique_id]) {
        for (int variable : patterns[pattern_id]) {
            assert(variable_to_pattern_id[variable] == -1);
            variable_to_pattern_id[variable] = pattern_id;
        }
    }

    dominated_patterns.clear();
    dominated_patterns.reserve(patterns.size());
    for (size_t i = 0; i < patterns.size(); ++i) {
        dominated_patterns.push_back(is_pattern_dominated(i));
    }
}

bool Pruner::is_pattern_dominated(int pattern_id) const {
    /*
      Check if the pattern with the given pattern_id is dominated
      by the current pattern clique.
    */
    const Pattern &pattern = patterns[pattern_id];
    assert(!pattern.empty());
    PatternID clique_pattern_id = variable_to_pattern_id[pattern[0]];
    if (clique_pattern_id == -1) {
        return false;
    }
    int pattern_size = pattern.size();
    for (int i = 1; i < pattern_size; ++i) {
        if (variable_to_pattern_id[pattern[i]] != clique_pattern_id) {
            return false;
        }
    }
    return true;
}

bool Pruner::is_clique_dominated(int clique_id) const {
    /*
      Check if the collection with the given collection_id is
      dominated by the current pattern collection.
    */
    for (PatternID pattern_id : pattern_cliques[clique_id]) {
        if (!dominated_patterns[pattern_id]) {
            return false;
        }
    }
    return true;
}

Pruner::Pruner(
    const PatternCollection &patterns,
    const vector<PatternClique> &pattern_cliques,
    int num_variables)
    : patterns(patterns),
      pattern_cliques(pattern_cliques),
      num_variables(num_variables) {
}

std::vector<bool> Pruner::get_pruned_cliques(
    const utils::CountdownTimer &timer) {
    int num_cliques = pattern_cliques.size();
    vector<bool> pruned(num_cliques, false);
    /*
      Already pruned cliques are not used to prune other
      cliques. This makes things faster and helps handle
      duplicate cliques in the correct way: the first copy
      will survive and prune all duplicates.
    */
    for (int c1 = 0; c1 < num_cliques; ++c1) {
        if (!pruned[c1]) {
            set_current_clique(c1);
            for (int c2 = 0; c2 < num_cliques; ++c2) {
                if (c1 != c2 && !pruned[c2] && is_clique_dominated(c2))
                    pruned[c2] = true;
            }
        }
        if (timer.is_expired()) {
            /*
              Since after each iteration, we determined if a given
              clique is pruned or not, we can just break the
              computation here if reaching the time limit and use all
              information we collected so far.
            */
            cout << "Time limit reached. Abort dominance pruning." << endl;
            break;
        }
    }

    return pruned;
}

}
