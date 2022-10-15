#include "pdbs/pattern_collection_generator_combo.h"

#include "pdbs/pattern_generator_greedy.h"
#include "pdbs/utils.h"
#include "pdbs/validation.h"

#include "utils/logging.h"
#include "utils/timer.h"

#include "globals.h"
#include "option_parser.h"
#include "plugin.h"

#include <iostream>
#include <memory>
#include <set>

using namespace std;

namespace pdbs {
PatternCollectionGeneratorCombo::PatternCollectionGeneratorCombo(const Options &opts)
    : max_states(opts.get<int>("max_states")) {
}

PatternCollectionInformation PatternCollectionGeneratorCombo::generate(OperatorCost cost_type) {
    utils::Timer timer;
    cout << "Generating patterns using the combo generator..." << endl;
    shared_ptr<PatternCollection> patterns = make_shared<PatternCollection>();

    PatternGeneratorGreedy large_pattern_generator(max_states);
    const Pattern &large_pattern = large_pattern_generator.generate(cost_type).get_pattern();
    patterns->push_back(large_pattern);

    set<int> used_vars(large_pattern.begin(), large_pattern.end());
    for (const auto& goal : g_goal) {
        int goal_var_id = goal.first;
        if (!used_vars.count(goal_var_id))
            patterns->emplace_back(1, goal_var_id);
    }

    PatternCollectionInformation pci(cost_type, patterns);
    dump_pattern_collection_generation_statistics(
        "Combo generator", timer(), pci);
    return pci;
}

static shared_ptr<PatternCollectionGenerator> _parse(OptionParser &parser) {
    parser.add_option<int>(
        "max_states",
        "maximum abstraction size for combo strategy",
        "1000000",
        Bounds("1", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;

    return make_shared<PatternCollectionGeneratorCombo>(opts);
}

static Plugin<PatternCollectionGenerator> _plugin("combo", _parse);
}
