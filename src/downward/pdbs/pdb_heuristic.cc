#include "downward/pdbs/pdb_heuristic.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/task_transformation.h"

#include <limits>
#include <memory>

using namespace std;

namespace downward::pdbs {
static shared_ptr<PatternDatabase> get_pdb_from_generator(
    const shared_ptr<AbstractTask>& task,
    const shared_ptr<PatternGenerator>& pattern_generator)
{
    PatternInformation pattern_info = pattern_generator->generate(task);
    return pattern_info.get_pdb();
}

PDBHeuristic::PDBHeuristic(
    const shared_ptr<PatternGenerator>& pattern,
    std::shared_ptr<AbstractTask> original_task,
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
    , pdb(get_pdb_from_generator(transformed_task, pattern))
{
}

PDBHeuristic::PDBHeuristic(
    const std::shared_ptr<PatternGenerator>& pattern_generator,
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : PDBHeuristic(
          pattern_generator,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
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
