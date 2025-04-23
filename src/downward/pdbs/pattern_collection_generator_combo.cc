#include "downward/pdbs/pattern_collection_generator_combo.h"

#include "downward/pdbs/pattern_generator_greedy.h"
#include "downward/pdbs/validation.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/logging.h"

#include <iostream>
#include <memory>
#include <set>

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorCombo::PatternCollectionGeneratorCombo(
    int max_states,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_states(max_states)
    , verbosity(verbosity)
{
}

string PatternCollectionGeneratorCombo::name() const
{
    return "combo pattern collection generator";
}

PatternCollectionInformation PatternCollectionGeneratorCombo::compute_patterns(
    const shared_ptr<AbstractTask>& task)
{
    shared_ptr<PatternCollection> patterns = make_shared<PatternCollection>();

    PatternGeneratorGreedy large_pattern_generator(max_states, verbosity);
    Pattern large_pattern =
        large_pattern_generator.generate(task).get_pattern();
    set<int> used_vars(large_pattern.begin(), large_pattern.end());
    patterns->push_back(std::move(large_pattern));

    for (FactPair goal : task->get_goals()) {
        if (const int goal_var = goal.var; !used_vars.contains(goal_var)) {
            patterns->emplace_back(1, goal_var);
        }
    }

    PatternCollectionInformation pci(*task, patterns, log);
    return pci;
}

} // namespace downward::pdbs
