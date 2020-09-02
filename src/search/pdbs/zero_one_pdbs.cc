#include "zero_one_pdbs.h"

#include "pattern_database.h"

#include "../globals.h"
#include "../global_operator.h"
#include "../global_state.h"
#include "../utils/logging.h"

#include <iostream>
#include <limits>
#include <memory>
#include <vector>

using namespace std;

namespace pdbs {
ZeroOnePDBs::ZeroOnePDBs(
    OperatorCost cost_type,
    const PatternCollection &patterns) {
    vector<int> remaining_operator_costs;
    remaining_operator_costs.reserve(g_operators.size());
    for (const GlobalOperator& op : g_operators)
        remaining_operator_costs.push_back(::get_adjusted_action_cost(op, cost_type));

    pattern_databases.reserve(patterns.size());
    for (const Pattern &pattern : patterns) {
        shared_ptr<PatternDatabase> pdb = make_shared<PatternDatabase>(
            pattern, false, remaining_operator_costs);

        /* Set cost of relevant operators to 0 for further iterations
           (action cost partitioning). */
        for (unsigned op_num = 0; op_num < g_operators.size(); op_num++) {
            const GlobalOperator& op = g_operators[op_num];
            if (pdb->is_operator_relevant(op))
                remaining_operator_costs[op_num] = 0;
        }

        pattern_databases.push_back(pdb);
    }
}


int ZeroOnePDBs::get_value(const GlobalState &state) const {
    /*
      Because we use cost partitioning, we can simply add up all
      heuristic values of all patterns in the pattern collection.
    */
    int h_val = 0;
    for (const shared_ptr<PatternDatabase> &pdb : pattern_databases) {
        int pdb_value = pdb->get_value(state);
        if (pdb_value == numeric_limits<int>::max())
            return numeric_limits<int>::max();
        h_val += pdb_value;
    }
    return h_val;
}

double ZeroOnePDBs::compute_approx_mean_finite_h() const {
    double approx_mean_finite_h = 0;
    for (const shared_ptr<PatternDatabase> &pdb : pattern_databases) {
        approx_mean_finite_h += pdb->compute_mean_finite_h();
    }
    return approx_mean_finite_h;
}

void ZeroOnePDBs::dump() const {
    for (const shared_ptr<PatternDatabase> &pdb : pattern_databases) {
        cout << pdb->get_pattern() << endl;
    }
}
}
