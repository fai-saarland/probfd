#include "downward/pruning/stubborn_sets_atom_centric.h"

#include "downward/utils/logging.h"

#include "downward/abstract_task.h"

#include <algorithm>
#include <limits>

using namespace std;

namespace downward::stubborn_sets_atom_centric {
StubbornSetsAtomCentric::StubbornSetsAtomCentric(
    bool use_sibling_shortcut,
    AtomSelectionStrategy atom_selection_strategy,
    utils::Verbosity verbosity)
    : StubbornSets(verbosity)
    , use_sibling_shortcut(use_sibling_shortcut)
    , atom_selection_strategy(atom_selection_strategy)
{
}

void StubbornSetsAtomCentric::initialize(const shared_ptr<AbstractTask>& task)
{
    StubbornSets::initialize(task);
    log << "pruning method: atom-centric stubborn sets" << endl;

    int num_variables = task->get_variables().size();
    marked_producers.reserve(num_variables);
    marked_consumers.reserve(num_variables);
    for (VariableProxy var : task->get_variables()) {
        marked_producers.emplace_back(var.get_domain_size(), false);
        marked_consumers.emplace_back(var.get_domain_size(), false);
    }

    if (use_sibling_shortcut) {
        marked_producer_variables.resize(num_variables, MARKED_VALUES_NONE);
        marked_consumer_variables.resize(num_variables, MARKED_VALUES_NONE);
    }

    compute_consumers(*task);
}

void StubbornSetsAtomCentric::compute_consumers(const AbstractTask& task)
{
    consumers.reserve(task.get_variables().size());
    for (VariableProxy var : task.get_variables()) {
        consumers.emplace_back(var.get_domain_size());
    }

    for (OperatorProxy op : task.get_operators()) {
        int op_id = op.get_id();
        for (const auto [var, value] : op.get_preconditions()) {
            consumers[var][value].push_back(op_id);
        }
    }

    for (auto& outer : consumers) {
        for (auto& inner : outer) { inner.shrink_to_fit(); }
    }
}

bool StubbornSetsAtomCentric::operator_is_applicable(int op, const State& state)
    const
{
    return find_unsatisfied_precondition(op, state) == FactPair::no_fact;
}

void StubbornSetsAtomCentric::enqueue_producers(const FactPair& fact)
{
    if (!marked_producers[fact.var][fact.value]) {
        marked_producers[fact.var][fact.value] = true;
        producer_queue.push_back(fact);
    }
}

void StubbornSetsAtomCentric::enqueue_consumers(const FactPair& fact)
{
    if (!marked_consumers[fact.var][fact.value]) {
        marked_consumers[fact.var][fact.value] = true;
        consumer_queue.push_back(fact);
    }
}

void StubbornSetsAtomCentric::enqueue_sibling_producers(const FactPair& fact)
{
    /*
      If we don't use the sibling shortcut handling, we ignore any
      variable-based marking info and always enqueue all sibling facts of the
      given fact v=d.
    */
    int dummy_mark = MARKED_VALUES_NONE;
    int& mark =
        use_sibling_shortcut ? marked_producer_variables[fact.var] : dummy_mark;
    if (mark == MARKED_VALUES_NONE) {
        /*
          If we don't have marking info for variable v, enqueue all sibling
          producers of v=d and remember that we marked all siblings.
        */
        int domain_size = consumers[fact.var].size();
        for (int value = 0; value < domain_size; ++value) {
            if (value != fact.value) {
                enqueue_producers(FactPair(fact.var, value));
            }
        }
        mark = fact.value;
    } else if (mark != MARKED_VALUES_ALL && mark != fact.value) {
        /*
          Exactly one fact v=d' has not been enqueued. It is therefore the only
          sibling of v=d that we need to enqueue.
        */
        enqueue_producers(FactPair(fact.var, mark));
        mark = MARKED_VALUES_ALL;
    }
}

void StubbornSetsAtomCentric::enqueue_sibling_consumers(const FactPair& fact)
{
    // For documentation, see enqueue_sibling_producers().
    int dummy_mark = MARKED_VALUES_NONE;
    int& mark =
        use_sibling_shortcut ? marked_consumer_variables[fact.var] : dummy_mark;
    if (mark == MARKED_VALUES_NONE) {
        int domain_size = consumers[fact.var].size();
        for (int value = 0; value < domain_size; ++value) {
            if (value != fact.value) {
                enqueue_consumers(FactPair(fact.var, value));
            }
        }
        mark = fact.value;
    } else if (mark != MARKED_VALUES_ALL && mark != fact.value) {
        enqueue_consumers(FactPair(fact.var, mark));
        mark = MARKED_VALUES_ALL;
    }
}

FactPair StubbornSetsAtomCentric::select_fact(
    const vector<FactPair>& facts,
    const State& state) const
{
    FactPair fact = FactPair::no_fact;
    if (atom_selection_strategy == AtomSelectionStrategy::FAST_DOWNWARD) {
        fact = stubborn_sets::find_unsatisfied_condition(facts, state);
    } else if (atom_selection_strategy == AtomSelectionStrategy::QUICK_SKIP) {
        /*
          If there is an unsatisfied fact whose producers are already marked,
          choose it. Otherwise, choose the first unsatisfied fact.
        */
        for (const FactPair& condition : facts) {
            if (state[condition.var] != condition.value) {
                if (marked_producers[condition.var][condition.value]) {
                    fact = condition;
                    break;
                } else if (fact == FactPair::no_fact) {
                    fact = condition;
                }
            }
        }
    } else if (atom_selection_strategy == AtomSelectionStrategy::STATIC_SMALL) {
        int min_count = numeric_limits<int>::max();
        for (const FactPair& condition : facts) {
            if (state[condition.var] != condition.value) {
                int count = achievers[condition.var][condition.value].size();
                if (count < min_count) {
                    fact = condition;
                    min_count = count;
                }
            }
        }
    } else if (
        atom_selection_strategy == AtomSelectionStrategy::DYNAMIC_SMALL) {
        int min_count = numeric_limits<int>::max();
        for (const FactPair& condition : facts) {
            if (state[condition.var] != condition.value) {
                const vector<int>& ops =
                    achievers[condition.var][condition.value];
                int count = std::ranges::count_if(ops, [&](int op) {
                    return !stubborn[op];
                });
                if (count < min_count) {
                    fact = condition;
                    min_count = count;
                }
            }
        }
    } else {
        ABORT("Unknown atom selection strategy");
    }
    assert(fact != FactPair::no_fact);
    return fact;
}

void StubbornSetsAtomCentric::enqueue_nes(int op, const State& state)
{
    FactPair fact = select_fact(sorted_op_preconditions[op], state);
    enqueue_producers(fact);
}

void StubbornSetsAtomCentric::enqueue_interferers(int op)
{
    for (const FactPair& fact : sorted_op_preconditions[op]) {
        // Enqueue operators that disable op.
        enqueue_sibling_producers(fact);
    }
    for (const FactPair& fact : sorted_op_effects[op]) {
        // Enqueue operators that conflict with op.
        enqueue_sibling_producers(fact);

        // Enqueue operators that op disables.
        enqueue_sibling_consumers(fact);
    }
}

void StubbornSetsAtomCentric::compute_stubborn_set(const State& state)
{
    assert(producer_queue.empty());
    assert(consumer_queue.empty());
    // Reset data structures from previous call.
    for (auto& facts : marked_producers) { facts.assign(facts.size(), false); }
    for (auto& facts : marked_consumers) { facts.assign(facts.size(), false); }
    if (use_sibling_shortcut) {
        int num_variables = state.size();
        marked_producer_variables.assign(num_variables, MARKED_VALUES_NONE);
        marked_consumer_variables.assign(num_variables, MARKED_VALUES_NONE);
    }

    FactPair unsatisfied_goal = select_fact(sorted_goals, state);
    assert(unsatisfied_goal != FactPair::no_fact);
    enqueue_producers(unsatisfied_goal);

    while (!producer_queue.empty() || !consumer_queue.empty()) {
        if (!producer_queue.empty()) {
            FactPair fact = producer_queue.back();
            producer_queue.pop_back();
            for (int op : achievers[fact.var][fact.value]) {
                handle_stubborn_operator(state, op);
            }
        } else {
            FactPair fact = consumer_queue.back();
            consumer_queue.pop_back();
            for (int op : consumers[fact.var][fact.value]) {
                handle_stubborn_operator(state, op);
            }
        }
    }
}

void StubbornSetsAtomCentric::handle_stubborn_operator(
    const State& state,
    int op)
{
    if (!stubborn[op]) {
        stubborn[op] = true;
        if (operator_is_applicable(op, state)) {
            enqueue_interferers(op);
        } else {
            enqueue_nes(op, state);
        }
    }
}

} // namespace downward::stubborn_sets_atom_centric
