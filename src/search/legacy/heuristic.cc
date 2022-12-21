#include "legacy/heuristic.h"

#include "legacy/global_operator.h"

#include "operator_cost.h"
#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <cstdlib>
#include <limits>

namespace legacy {

using namespace std;

Heuristic::Heuristic(const options::Options& opts)
    : cost_type(opts.get<OperatorCost>("cost_type"))
{
    heuristic = NOT_INITIALIZED;
}

Heuristic::~Heuristic()
{
}

void Heuristic::ensure_initialized()
{
    if (heuristic == NOT_INITIALIZED) {
        initialize();
        heuristic = 0;
    }
}

void Heuristic::set_preferred(const GlobalOperator* op)
{
    if (!op->is_marked()) {
        op->mark();
        preferred_operators.push_back(op);
    }
}

void Heuristic::evaluate(const GlobalState& state)
{
    if (heuristic == NOT_INITIALIZED) initialize();
    preferred_operators.clear();
    heuristic = compute_heuristic(state);
    for (size_t i = 0; i < preferred_operators.size(); ++i)
        preferred_operators[i]->unmark();
    assert(heuristic == DEAD_END || heuristic >= 0);

    if (heuristic == DEAD_END) {
        // It is ok to have preferred operators in dead-end states.
        // This allows a heuristic to mark preferred operators on-the-fly,
        // selecting the first ones before it is clear that all goals
        // can be reached.
        preferred_operators.clear();
    }

#ifndef NDEBUG
    if (heuristic != DEAD_END) {
        for (size_t i = 0; i < preferred_operators.size(); ++i)
            assert(preferred_operators[i]->is_applicable(state));
    }
#endif
    evaluator_value = heuristic;
}

bool Heuristic::is_dead_end() const
{
    return evaluator_value == DEAD_END;
}

int Heuristic::get_heuristic()
{
    // The -1 value for dead ends is an implementation detail which is
    // not supposed to leak. Thus, calling this for dead ends is an
    // error. Call "is_dead_end()" first.

    /*
      TODO: I've commented the assertion out for now because there is
      currently code that calls get_heuristic for dead ends. For
      example, if we use alternation with h^FF and h^cea and have an
      instance where the initial state has infinite h^cea value, we
      should expand this state since h^cea is unreliable. The search
      progress class will then want to print the h^cea value of the
      initial state since this is the "best know h^cea state" so far.

      However, we should clean up the code again so that the assertion
      is valid or rethink the interface so that we don't need it.
     */

    // assert(heuristic >= 0);
    if (heuristic == DEAD_END) return numeric_limits<int>::max();
    return heuristic;
}

void Heuristic::get_preferred_operators(
    std::vector<const GlobalOperator*>& result)
{
    assert(heuristic >= 0);
    result.insert(
        result.end(),
        preferred_operators.begin(),
        preferred_operators.end());
}

bool Heuristic::reach_state(
    const GlobalState& /*parent_state*/,
    const GlobalOperator& /*op*/,
    const GlobalState& /*state*/)
{
    return false;
}

int Heuristic::get_value() const
{
    return evaluator_value;
}

void Heuristic::evaluate(int, bool)
{
    return;
    // if this is called, evaluate(const GlobalState &state) or
    // set_evaluator_value(int val) should already have been called
}

bool Heuristic::dead_end_is_reliable() const
{
    return dead_ends_are_reliable();
}

void Heuristic::set_evaluator_value(int val)
{
    evaluator_value = val;
}

int Heuristic::get_adjusted_cost(const GlobalOperator& op) const
{
    return get_adjusted_action_cost(op.get_cost(), cost_type);
}

void Heuristic::add_options_to_parser(options::OptionParser& parser)
{
    ::add_cost_type_option_to_parser(parser);
}

bool Heuristic::supports_partial_state_evaluation() const
{
    return false;
}

int Heuristic::compute_heuristic(const PartialState&)
{
    return 0;
}

void Heuristic::evaluate(const PartialState& partial_state)
{
    assert(supports_partial_state_evaluation());
    if (heuristic == NOT_INITIALIZED) initialize();
    preferred_operators.clear();
    heuristic = compute_heuristic(partial_state);
    for (size_t i = 0; i < preferred_operators.size(); ++i)
        preferred_operators[i]->unmark();
    assert(heuristic == DEAD_END || heuristic >= 0);
    if (heuristic == DEAD_END) {
        // It is ok to have preferred operators in dead-end states.
        // This allows a heuristic to mark preferred operators on-the-fly,
        // selecting the first ones before it is clear that all goals
        // can be reached.
        preferred_operators.clear();
    }
    evaluator_value = heuristic;
}

// this solution to get default values seems not optimal:
options::Options Heuristic::default_options()
{
    options::Options opts = options::Options();
    opts.set<int>("cost_type", 0);
    return opts;
}

static PluginTypePlugin<Heuristic> _type_plugin(
    "LegacyHeuristic",
    "An evaluator specification is either a newly created evaluator "
    "instance or an evaluator that has been defined previously. "
    "This page describes how one can specify a new evaluator instance. "
    "For re-using evaluators, see "
    "options::OptionSyntax#Evaluator_Predefinitions.\n\n"
    "If the evaluator is a heuristic, "
    "definitions of //properties// in the descriptions below:\n\n"
    " * **admissible:** h(s) <= h*(s) for all states s\n"
    " * **consistent:** h(s) <= c(s, s') + h(s') for all states s "
    "connected to states s' by an action with cost c(s, s')\n"
    " * **safe:** h(s) = infinity is only true for states "
    "with h*(s) = infinity\n"
    " * **preferred operators:** this heuristic identifies ",
    "legacy_heuristic");

} // namespace legacy