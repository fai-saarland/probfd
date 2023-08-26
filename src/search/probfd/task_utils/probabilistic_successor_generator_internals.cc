#include "probfd/task_utils/probabilistic_successor_generator_internals.h"

#include "downward/task_proxy.h"

#include "probfd/task_state_space.h"
#include "probfd/transition.h"

#include <cassert>

using namespace std;

/*
  Notes on possible optimizations:

  - Using specialized allocators (e.g. an arena allocator) could
    improve cache locality and reduce memory.

  - We could keep the different nodes in a single vector (for example
    of type unique_ptr<ProbabilisticGeneratorBase>) and then use indices rather
    than pointers for representing child nodes. This would reduce the
    memory overhead for pointers in 64-bit builds. However, this
    overhead is not as bad as it used to be.

  - Going further down this route, on the more extreme end of the
    spectrum, we could use a "byte-code" style representation, where
    the successor generator is just a long vector of ints combining
    information about node type with node payload.

    For example, we could represent different node types as follows,
    where BINARY_FORK etc. are symbolic constants for tagging node
    types:

    - binary fork: [BINARY_FORK, child_1, child_2]
    - multi-fork:  [MULTI_FORK, n, child_1, ..., child_n]
    - vector switch: [VECTOR_SWITCH, var_id, child_1, ..., child_k]
    - single switch: [SINGLE_SWITCH, var_id, value, child_index]
    - hash switch: [HASH_SWITCH, var_id, map_no]
      where map_no is an index into a separate vector of hash maps
      (represented out of band)
    - single leaf: [SINGLE_LEAF, op_id]
    - vector leaf: [VECTOR_LEAF, n, op_id_1, ..., op_id_n]

    We could compact this further by permitting to use operator IDs
    directly wherever child nodes are used, by using e.g. negative
    numbers for operatorIDs and positive numbers for node IDs,
    obviating the need for SINGLE_LEAF. This would also make
    VECTOR_LEAF redundant, as MULTI_FORK could be used instead.

    Further, if the other symbolic constants are negative numbers,
    we could represent forks just as [n, child_1, ..., child_n] without
    symbolic constant at the start, unifying binary and multi-forks.

    To completely unify the representation, not needing hash values
    out of band, we might consider switches of the form [SWITCH, k,
    var_id, value_1, child_1, ..., value_k, child_k] that permit
    binary searching. This would only leave switch and fork nodes, and
    we could do away with the type tags by just using +k for one node
    type and -k for the other. (But it may be useful to leave the
    possibility of the current vector switches for very dense switch
    nodes, which could be used in the case where k equals the domain
    size of the variable in question.)

  - More modestly, we could stick with the current polymorphic code,
    but just use more types of nodes, such as switch nodes that stores
    a vector of (value, child) pairs to be scanned linearly or with
    binary search.

  - We can also try to optimize memory usage of the existing nodes
    further, e.g. by replacing vectors with something smaller, like a
    zero-terminated heap-allocated array.
*/

namespace probfd {
namespace successor_generator {
ProbabilisticGeneratorForkBinary::ProbabilisticGeneratorForkBinary(
    unique_ptr<ProbabilisticGeneratorBase> generator1,
    unique_ptr<ProbabilisticGeneratorBase> generator2)
    : generator1(std::move(generator1))
    , generator2(std::move(generator2))
{
    /* There is no reason to use a fork if only one of the generators exists.
       Use the existing generator directly if one of them exists or a nullptr
       otherwise. */
    assert(this->generator1);
    assert(this->generator2);
}

void ProbabilisticGeneratorForkBinary::generate_applicable_ops(
    const vector<int>& state,
    vector<OperatorID>& applicable_ops) const
{
    generator1->generate_applicable_ops(state, applicable_ops);
    generator2->generate_applicable_ops(state, applicable_ops);
}

void ProbabilisticGeneratorForkBinary::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    generator1->generate_transitions(state, transitions, task_state_space);
    generator2->generate_transitions(state, transitions, task_state_space);
}

ProbabilisticGeneratorForkMulti::ProbabilisticGeneratorForkMulti(
    vector<unique_ptr<ProbabilisticGeneratorBase>> children)
    : children(std::move(children))
{
    /* Note that we permit 0-ary forks as a way to define empty
       successor generators (for tasks with no operators). It is
       the responsibility of the factory code to make sure they
       are not generated in other circumstances. */
    assert(this->children.empty() || this->children.size() >= 2);
}

void ProbabilisticGeneratorForkMulti::generate_applicable_ops(
    const vector<int>& state,
    vector<OperatorID>& applicable_ops) const
{
    for (const auto& generator : children)
        generator->generate_applicable_ops(state, applicable_ops);
}

void ProbabilisticGeneratorForkMulti::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    for (const auto& generator : children)
        generator->generate_transitions(state, transitions, task_state_space);
}

ProbabilisticGeneratorSwitchVector::ProbabilisticGeneratorSwitchVector(
    int switch_var_id,
    vector<unique_ptr<ProbabilisticGeneratorBase>>&& generator_for_value)
    : switch_var_id(switch_var_id)
    , generator_for_value(std::move(generator_for_value))
{
}

void ProbabilisticGeneratorSwitchVector::generate_applicable_ops(
    const vector<int>& state,
    vector<OperatorID>& applicable_ops) const
{
    int val = state[switch_var_id];
    const unique_ptr<ProbabilisticGeneratorBase>& generator_for_val =
        generator_for_value[val];
    if (generator_for_val) {
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

void ProbabilisticGeneratorSwitchVector::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    int val = state.get_unpacked_values()[switch_var_id];
    const unique_ptr<ProbabilisticGeneratorBase>& generator_for_val =
        generator_for_value[val];
    if (generator_for_val) {
        generator_for_val->generate_transitions(
            state,
            transitions,
            task_state_space);
    }
}

ProbabilisticGeneratorSwitchHash::ProbabilisticGeneratorSwitchHash(
    int switch_var_id,
    unordered_map<int, unique_ptr<ProbabilisticGeneratorBase>>&&
        generator_for_value)
    : switch_var_id(switch_var_id)
    , generator_for_value(std::move(generator_for_value))
{
}

void ProbabilisticGeneratorSwitchHash::generate_applicable_ops(
    const vector<int>& state,
    vector<OperatorID>& applicable_ops) const
{
    int val = state[switch_var_id];
    const auto& child = generator_for_value.find(val);
    if (child != generator_for_value.end()) {
        const unique_ptr<ProbabilisticGeneratorBase>& generator_for_val =
            child->second;
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

void ProbabilisticGeneratorSwitchHash::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    int val = state.get_unpacked_values()[switch_var_id];
    const auto& child = generator_for_value.find(val);
    if (child != generator_for_value.end()) {
        const unique_ptr<ProbabilisticGeneratorBase>& generator_for_val =
            child->second;
        generator_for_val->generate_transitions(
            state,
            transitions,
            task_state_space);
    }
}

ProbabilisticGeneratorSwitchSingle::ProbabilisticGeneratorSwitchSingle(
    int switch_var_id,
    int value,
    unique_ptr<ProbabilisticGeneratorBase> generator_for_value)
    : switch_var_id(switch_var_id)
    , value(value)
    , generator_for_value(std::move(generator_for_value))
{
}

void ProbabilisticGeneratorSwitchSingle::generate_applicable_ops(
    const vector<int>& state,
    vector<OperatorID>& applicable_ops) const
{
    if (value == state[switch_var_id]) {
        generator_for_value->generate_applicable_ops(state, applicable_ops);
    }
}

void ProbabilisticGeneratorSwitchSingle::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    if (value == state.get_unpacked_values()[switch_var_id]) {
        generator_for_value->generate_transitions(
            state,
            transitions,
            task_state_space);
    }
}

ProbabilisticGeneratorLeafVector::ProbabilisticGeneratorLeafVector(
    vector<OperatorID>&& applicable_operators)
    : applicable_operators(std::move(applicable_operators))
{
}

void ProbabilisticGeneratorLeafVector::generate_applicable_ops(
    const vector<int>&,
    vector<OperatorID>& applicable_ops) const
{
    /*
      In our experiments (issue688), a loop over push_back was faster
      here than doing this with a single insert call because the
      containers are typically very small. However, we have changed
      the container type from list to vector since then, so this might
      no longer apply.
    */
    for (OperatorID id : applicable_operators) {
        applicable_ops.push_back(id);
    }
}

void ProbabilisticGeneratorLeafVector::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    for (OperatorID id : applicable_operators) {
        auto& t = transitions.emplace_back(id);
        task_state_space.compute_successor_dist(state, id, t.successor_dist);
    }
}

ProbabilisticGeneratorLeafSingle::ProbabilisticGeneratorLeafSingle(
    OperatorID applicable_operator)
    : applicable_operator(applicable_operator)
{
}

void ProbabilisticGeneratorLeafSingle::generate_applicable_ops(
    const vector<int>&,
    vector<OperatorID>& applicable_ops) const
{
    applicable_ops.push_back(applicable_operator);
}

void ProbabilisticGeneratorLeafSingle::generate_transitions(
    const State& state,
    std::vector<Transition<OperatorID>>& transitions,
    TaskStateSpace& task_state_space) const
{
    auto& t = transitions.emplace_back(applicable_operator);
    task_state_space.compute_successor_dist(
        state,
        applicable_operator,
        t.successor_dist);
}

} // namespace successor_generator
} // namespace probfd