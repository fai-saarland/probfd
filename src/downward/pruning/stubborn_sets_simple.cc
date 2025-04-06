#include "downward/pruning/stubborn_sets_simple.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::stubborn_sets_simple {
StubbornSetsSimple::StubbornSetsSimple(utils::Verbosity verbosity)
    : StubbornSetsActionCentric(verbosity)
{
}

void StubbornSetsSimple::initialize(const shared_ptr<AbstractTask>& task)
{
    StubbornSets::initialize(task);
    interference_relation.resize(num_operators);
    interference_relation_computed.resize(num_operators, false);
    log << "pruning method: stubborn sets simple" << endl;
}

const vector<int>& StubbornSetsSimple::get_interfering_operators(int op1_no)
{
    /*
       TODO: as interference is symmetric, we only need to compute the
       relation for operators (o1, o2) with (o1 < o2) and add a lookup
       method that looks up (i, j) if i < j and (j, i) otherwise.
    */
    vector<int>& interfere_op1 = interference_relation[op1_no];
    if (!interference_relation_computed[op1_no]) {
        for (int op2_no = 0; op2_no < num_operators; ++op2_no) {
            if (op1_no != op2_no && interfere(op1_no, op2_no)) {
                interfere_op1.push_back(op2_no);
            }
        }
        interfere_op1.shrink_to_fit();
        interference_relation_computed[op1_no] = true;
    }
    return interfere_op1;
}

// Add all operators that achieve the fact (var, value) to stubborn set.
void StubbornSetsSimple::add_necessary_enabling_set(const FactPair& fact)
{
    for (int op_no : achievers[fact.var][fact.value]) {
        enqueue_stubborn_operator(op_no);
    }
}

// Add all operators that interfere with op.
void StubbornSetsSimple::add_interfering(int op_no)
{
    for (int interferer_no : get_interfering_operators(op_no)) {
        enqueue_stubborn_operator(interferer_no);
    }
}

void StubbornSetsSimple::initialize_stubborn_set(const State& state)
{
    // Add a necessary enabling set for an unsatisfied goal.
    FactPair unsatisfied_goal = find_unsatisfied_goal(state);
    assert(unsatisfied_goal != FactPair::no_fact);
    add_necessary_enabling_set(unsatisfied_goal);
}

void StubbornSetsSimple::handle_stubborn_operator(const State& state, int op_no)
{
    FactPair unsatisfied_precondition =
        find_unsatisfied_precondition(op_no, state);
    if (unsatisfied_precondition == FactPair::no_fact) {
        /* no unsatisfied precondition found
           => operator is applicable
           => add all interfering operators */
        add_interfering(op_no);
    } else {
        /* unsatisfied precondition found
           => add a necessary enabling set for it */
        add_necessary_enabling_set(unsatisfied_precondition);
    }
}

} // namespace stubborn_sets_simple
