#include "downward/pdbs/zero_one_pdbs_heuristic.h"

#include "downward/task_transformation.h"

#include <limits>

using namespace std;

namespace downward::pdbs {
static ZeroOnePDBs get_zero_one_pdbs_from_generator(
    const SharedAbstractTask& task,
    const shared_ptr<PatternCollectionGenerator>& pattern_generator)
{
    PatternCollectionInformation pattern_collection_info =
        pattern_generator->generate(task);
    shared_ptr<PatternCollection> patterns =
        pattern_collection_info.get_patterns();
    return ZeroOnePDBs(to_refs(task), *patterns);
}

ZeroOnePDBsHeuristic::ZeroOnePDBsHeuristic(
    const shared_ptr<PatternCollectionGenerator>& patterns,
    SharedAbstractTask original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
    , zero_one_pdbs(
          get_zero_one_pdbs_from_generator(transformed_task, patterns))
{
}

ZeroOnePDBsHeuristic::ZeroOnePDBsHeuristic(
    const std::shared_ptr<PatternCollectionGenerator>& patterns,
    SharedAbstractTask original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& name,
    utils::Verbosity verbosity)
    : ZeroOnePDBsHeuristic(
          patterns,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          name,
          verbosity)
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
