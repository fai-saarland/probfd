#include "downward/pdbs/pattern_collection_generator_combo.h"

#include "downward/pdbs/pattern_generator_greedy.h"
#include "downward/pdbs/validation.h"

#include "downward/task_proxy.h"

#include "downward/utils/logging.h"

#include <iostream>
#include <memory>
#include <set>

using namespace std;

namespace pdbs {
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
    TaskProxy task_proxy(*task);
    shared_ptr<PatternCollection> patterns = make_shared<PatternCollection>();

    PatternGeneratorGreedy large_pattern_generator(max_states, verbosity);
    Pattern large_pattern =
        large_pattern_generator.generate(task).get_pattern();
    set<int> used_vars(large_pattern.begin(), large_pattern.end());
    patterns->push_back(std::move(large_pattern));

    for (FactProxy goal : task_proxy.get_goals()) {
        int goal_var_id = goal.get_variable().get_id();
        if (!used_vars.count(goal_var_id)) {
            patterns->emplace_back(1, goal_var_id);
        }
    }

    PatternCollectionInformation pci(task_proxy, patterns, log);
    return pci;
}

} // namespace pdbs
