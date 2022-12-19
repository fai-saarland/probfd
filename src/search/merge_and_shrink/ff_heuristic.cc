#include "merge_and_shrink/ff_heuristic.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "option_parser.h"

#include <cassert>
#include <vector>
using namespace std;

namespace merge_and_shrink {

// construction and destruction
FFHeuristic::FFHeuristic(const options::Options& opts)
    : AdditiveHeuristic(opts)
    , c_tiebreaking(opts.get<TieBreaking>("ties"))
    , m_rng(opts.get<int>("seed"))
{
}

FFHeuristic::~FFHeuristic() {
}

// initialization
void FFHeuristic::initialize() {
    cout << "Initializing FF heuristic..." << endl;
    AdditiveHeuristic::initialize();
    relaxed_plan.resize(g_operators.size(), false);
}

void FFHeuristic::mark_preferred_operators_and_relaxed_plan(
    const GlobalState &state, Proposition *goal) {
    if (!goal->marked) { // Only consider each subgoal once.
        goal->marked = true;
        UnaryOperator *unary_op = goal->reached_by;
        if (unary_op) { // We have not yet chained back to a start node.
            for (unsigned i = 0; i < unary_op->precondition.size(); i++)
                mark_preferred_operators_and_relaxed_plan(
                    state, unary_op->precondition[i]);
            int operator_no = unary_op->operator_no;
            if (operator_no != -1) {
                // This is not an axiom.
                relaxed_plan[operator_no] = true;

                if (unary_op->cost == unary_op->base_cost) {
                    // This test is implied by the next but cheaper,
                    // so we perform it to save work.
                    // If we had no 0-cost operators and axioms to worry
                    // about, it would also imply applicability.
                    const GlobalOperator *op = &g_operators[operator_no];
                    if (op->is_applicable(state))
                        set_preferred(op);
                }
            }
        }
    }
}

int FFHeuristic::compute_heuristic(const GlobalState &state) {
    int h_add = compute_add_and_ff(state);
    if (h_add == DEAD_END)
        return h_add;

    std::vector<std::vector<UnaryOperator *> > m_achievers;
    m_achievers.resize(m_propositions);
    for (unsigned i = 0; i < unary_operators.size(); i++) {
      UnaryOperator &op = unary_operators[i];
      if (op.unsatisfied_preconditions == 0
          && op.effect->cost == op.cost) {
        m_achievers[op.effect->id].push_back(&op);
      }
    }
    unsigned prop_id = 0;
    for (unsigned var = 0; var < g_variable_domain.size(); var++) {
      for (int val = 0; val < g_variable_domain[var]; val++) {
        if (!m_achievers[prop_id].empty()) {
          switch(c_tiebreaking) {
          case(ARBITRARY):
            propositions[var][val].reached_by = m_achievers[prop_id][0];
            break;
          case(RANDOM):
            propositions[var][val].reached_by = m_achievers[prop_id][m_rng(m_achievers[prop_id].size())];
            break;
          }
        }
        prop_id++;
      }
    }
    m_achievers.clear();

    // Collecting the relaxed plan also sets the preferred operators.
    for (unsigned i = 0; i < goal_propositions.size(); i++)
        mark_preferred_operators_and_relaxed_plan(state, goal_propositions[i]);

    int h_ff = 0;
    for (unsigned op_no = 0; op_no < relaxed_plan.size(); op_no++) {
        if (relaxed_plan[op_no]) {
            relaxed_plan[op_no] = false; // Clean up for next computation.
            h_ff += get_adjusted_cost(g_operators[op_no]);
        }
    }
    return h_ff;
}


void FFHeuristic::mark_relaxed_plan(const GlobalState &state, Proposition *goal) {
    if (!goal->marked) { // Only consider each subgoal once.
        goal->marked = true;
        UnaryOperator *unary_op = goal->reached_by;
        if (unary_op) { // We have not yet chained back to a start node.
            for (unsigned i = 0; i < unary_op->precondition.size(); i++)
                mark_relaxed_plan(state, unary_op->precondition[i]);
            int operator_no = unary_op->operator_no;
            if (operator_no != -1) {
                // This is not an axiom.
                relaxed_plan[operator_no] = true;
            }
        }
    }
}

void FFHeuristic::compute_relaxed_plan(const GlobalState &state, RelaxedPlan &returned_relaxed_plan) {
	returned_relaxed_plan.clear();
	returned_relaxed_plan.resize(g_operators.size(), false);
	int h_add = compute_add_and_ff(state);
	if (h_add == DEAD_END)
		return;

	// mark only relaxed plan, no preferred operators
	for (unsigned i = 0; i < goal_propositions.size(); i++)
		mark_relaxed_plan(state, goal_propositions[i]);

	returned_relaxed_plan = relaxed_plan;
	relaxed_plan.assign(g_operators.size(), false);
}

}
