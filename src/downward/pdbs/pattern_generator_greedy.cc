#include "downward/pdbs/pattern_generator_greedy.h"

#include "downward/pdbs/pattern_information.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/task_utils/variable_order_finder.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"

#include <iostream>

using namespace std;

namespace downward::pdbs {
PatternGeneratorGreedy::PatternGeneratorGreedy(
    int max_states,
    utils::Verbosity verbosity)
    : PatternGenerator(verbosity)
    , max_states(max_states)
{
}

string PatternGeneratorGreedy::name() const
{
    return "greedy pattern generator";
}

PatternInformation
PatternGeneratorGreedy::compute_pattern(const shared_ptr<AbstractTask>& task)
{
    Pattern pattern;
    variable_order_finder::VariableOrderFinder order(
        *task,
        variable_order_finder::GOAL_CG_LEVEL);
    VariablesProxy variables = task->get_variables();

    int size = 1;
    while (true) {
        if (order.done()) break;
        int next_var_id = order.next();
        VariableProxy next_var = variables[next_var_id];
        int next_var_size = next_var.get_domain_size();

        if (!utils::is_product_within_limit(size, next_var_size, max_states))
            break;

        pattern.push_back(next_var_id);
        size *= next_var_size;
    }

    return PatternInformation(*task, std::move(pattern), log);
}

} // namespace downward::pdbs
