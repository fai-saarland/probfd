#include "pdbs/pattern_generator_greedy.h"

#include "pdbs/pattern_information.h"
#include "pdbs/utils.h"

#include "utils/logging.h"
#include "utils/math.h"
#include "utils/timer.h"

#include "globals.h"
#include "option_parser.h"
#include "plugin.h"
#include "variable_order_finder.h"


#include <iostream>

using namespace std;

namespace pdbs {
PatternGeneratorGreedy::PatternGeneratorGreedy(const Options &opts)
    : PatternGeneratorGreedy(opts.get<int>("max_states")) {
}

PatternGeneratorGreedy::PatternGeneratorGreedy(int max_states)
    : max_states(max_states) {
}

PatternInformation PatternGeneratorGreedy::generate(OperatorCost cost_type) {
    utils::Timer timer;
    cout << "Generating a pattern using the greedy generator..." << endl;
    Pattern pattern;
    variable_order_finder::VariableOrderFinder order(variable_order_finder::GOAL_CG_LEVEL);

    int size = 1;
    while (true) {
        if (order.done())
            break;
        int next_var_id = order.next();
        int next_var_size = g_variable_domain[next_var_id];

        if (!utils::is_product_within_limit(size, next_var_size, max_states))
            break;

        pattern.push_back(next_var_id);
        size *= next_var_size;
    }

    PatternInformation pattern_info(cost_type, move(pattern));
    dump_pattern_generation_statistics(
        "Greedy generator", timer(), pattern_info);
    return pattern_info;
}

static shared_ptr<PatternGenerator> _parse(OptionParser &parser) {
    parser.add_option<int>(
        "max_states",
        "maximal number of abstract states in the pattern database.",
        "1000000",
        Bounds("1", "infinity"));

    Options opts = parser.parse();
    if (parser.dry_run())
        return nullptr;

    return make_shared<PatternGeneratorGreedy>(opts);
}

static Plugin<PatternGenerator> _plugin("greedy", _parse);
}
