#include "downward/pdbs/canonical_pdbs_heuristic.h"

#include "downward/pdbs/dominance_pruning.h"
#include "downward/pdbs/utils.h"

#include "downward/utils/logging.h"
#include "downward/utils/timer.h"

#include <iostream>
#include <limits>
#include <memory>

using namespace std;

namespace downward::pdbs {
static CanonicalPDBs get_canonical_pdbs(
    const shared_ptr<AbstractTask>& task,
    const shared_ptr<PatternCollectionGenerator>& pattern_generator,
    double max_time_dominance_pruning,
    utils::LogProxy& log)
{
    utils::Timer timer;
    if (log.is_at_least_normal()) {
        log << "Initializing canonical PDB heuristic..." << endl;
    }
    PatternCollectionInformation pattern_collection_info =
        pattern_generator->generate(task);
    shared_ptr<PatternCollection> patterns =
        pattern_collection_info.get_patterns();
    /*
      We compute PDBs and pattern cliques here (if they have not been
      computed before) so that their computation is not taken into account
      for dominance pruning time.
    */
    shared_ptr<PDBCollection> pdbs = pattern_collection_info.get_pdbs();
    shared_ptr<vector<PatternClique>> pattern_cliques =
        pattern_collection_info.get_pattern_cliques();

    if (max_time_dominance_pruning > 0.0) {
        int num_variables = TaskProxy(*task).get_variables().size();
        /*
          NOTE: Dominance pruning could also be computed without having access
          to the PDBs, but since we want to delete patterns, we also want to
          update the list of corresponding PDBs so they are synchronized.

          In the long term, we plan to have patterns and their PDBs live
          together, in which case we would only need to pass their container
          and the pattern cliques.
        */
        prune_dominated_cliques(
            *patterns,
            *pdbs,
            *pattern_cliques,
            num_variables,
            max_time_dominance_pruning,
            log);
    }

    dump_pattern_collection_generation_statistics(
        "Canonical PDB heuristic",
        timer(),
        pattern_collection_info,
        log);
    return CanonicalPDBs(pdbs, pattern_cliques);
}

CanonicalPDBsHeuristic::CanonicalPDBsHeuristic(
    const shared_ptr<PatternCollectionGenerator>& patterns,
    double max_time_dominance_pruning,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , canonical_pdbs(
          get_canonical_pdbs(task, patterns, max_time_dominance_pruning, log))
{
}

int CanonicalPDBsHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int h = canonical_pdbs.get_value(state);
    if (h == numeric_limits<int>::max()) {
        return DEAD_END;
    } else {
        return h;
    }
}

} // namespace pdbs
