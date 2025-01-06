#include "downward/pdbs/pdb_heuristic.h"

#include "downward/pdbs/pattern_database.h"

#include <limits>
#include <memory>

using namespace std;

namespace pdbs {
static shared_ptr<PatternDatabase> get_pdb_from_generator(
    const shared_ptr<AbstractTask>& task,
    const shared_ptr<PatternGenerator>& pattern_generator)
{
    PatternInformation pattern_info = pattern_generator->generate(task);
    return pattern_info.get_pdb();
}

PDBHeuristic::PDBHeuristic(
    const shared_ptr<PatternGenerator>& pattern,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , pdb(get_pdb_from_generator(task, pattern))
{
}

int PDBHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int h = pdb->get_value(state.get_unpacked_values());
    if (h == numeric_limits<int>::max()) return DEAD_END;
    return h;
}

} // namespace pdbs
