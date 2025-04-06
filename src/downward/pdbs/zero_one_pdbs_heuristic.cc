#include "downward/pdbs/zero_one_pdbs_heuristic.h"

#include <limits>

using namespace std;

namespace downward::pdbs {
static ZeroOnePDBs get_zero_one_pdbs_from_generator(
    const shared_ptr<AbstractTask>& task,
    const shared_ptr<PatternCollectionGenerator>& pattern_generator)
{
    PatternCollectionInformation pattern_collection_info =
        pattern_generator->generate(task);
    shared_ptr<PatternCollection> patterns =
        pattern_collection_info.get_patterns();
    TaskProxy task_proxy(*task);
    return ZeroOnePDBs(task_proxy, *patterns);
}

ZeroOnePDBsHeuristic::ZeroOnePDBsHeuristic(
    const shared_ptr<PatternCollectionGenerator>& patterns,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , zero_one_pdbs(get_zero_one_pdbs_from_generator(task, patterns))
{
}

int ZeroOnePDBsHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int h = zero_one_pdbs.get_value(state);
    if (h == numeric_limits<int>::max()) return DEAD_END;
    return h;
}

} // namespace pdbs
