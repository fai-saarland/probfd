#ifdef LEGACY_SUCCESSOR_GENERATOR_INTERNALS_H

#include "legacy/global_state.h"

#include <cassert>

/*
  Notes on possible optimizations:

  - Using specialized allocators (e.g. an arena allocator) could
    improve cache locality and reduce memory.

  - We could keep the different nodes in a single vector (for example
    of type unique_ptr<GeneratorBase>) and then use indices rather
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

namespace legacy {
namespace successor_generator {

template <typename Entry>
GeneratorForkBinary<Entry>::GeneratorForkBinary(
    std::unique_ptr<GeneratorBase<Entry>> generator1,
    std::unique_ptr<GeneratorBase<Entry>> generator2)
    : generator1(move(generator1))
    , generator2(move(generator2))
{
    /* There is no reason to use a fork if only one of the generators exists.
       Use the existing generator directly if one of them exists or a nullptr
       otherwise. */
    assert(this->generator1);
    assert(this->generator2);
}

template <typename Entry>
void GeneratorForkBinary<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    generator1->generate_applicable_ops(state, applicable_ops);
    generator2->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void GeneratorForkBinary<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    generator1->generate_applicable_ops(state, applicable_ops);
    generator2->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void GeneratorForkBinary<Entry>::generate_applicable_ops(
    unsigned i,
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    generator1->generate_applicable_ops(i, facts, applicable_ops);
    generator2->generate_applicable_ops(i, facts, applicable_ops);
}

template <typename Entry>
GeneratorForkMulti<Entry>::GeneratorForkMulti(
    std::vector<std::unique_ptr<GeneratorBase<Entry>>> children)
    : children(move(children))
{
    /* Note that we permit 0-ary forks as a way to define empty
       successor generators (for tasks with no operators). It is
       the responsibility of the factory code to make sure they
       are not generated in other circumstances. */
    assert(this->children.empty() || this->children.size() >= 2);
}

template <typename Entry>
void GeneratorForkMulti<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    for (const auto& generator : children)
        generator->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void GeneratorForkMulti<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    for (const auto& generator : children)
        generator->generate_applicable_ops(state, applicable_ops);
}

template <typename Entry>
void GeneratorForkMulti<Entry>::generate_applicable_ops(
    unsigned i,
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    for (const auto& generator : children)
        generator->generate_applicable_ops(i, facts, applicable_ops);
}

template <typename Entry>
GeneratorSwitchVector<Entry>::GeneratorSwitchVector(
    int switch_var_id,
    std::vector<std::unique_ptr<GeneratorBase<Entry>>>&& generator_for_value)
    : switch_var_id(switch_var_id)
    , generator_for_value(move(generator_for_value))
{
}

template <typename Entry>
void GeneratorSwitchVector<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    int val = state[switch_var_id];
    const std::unique_ptr<GeneratorBase<Entry>>& generator_for_val =
        generator_for_value[val];
    if (generator_for_val) {
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchVector<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    int val = state[switch_var_id];
    const std::unique_ptr<GeneratorBase<Entry>>& generator_for_val =
        generator_for_value[val];
    if (generator_for_val) {
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchVector<Entry>::generate_applicable_ops(
    unsigned i,
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    while (i < facts.size() && facts[i].first < switch_var_id) {
        ++i;
    }
    if (i == facts.size() || facts[i].first > switch_var_id) {
        for (unsigned val = 0; val < generator_for_value.size(); ++val) {
            if (generator_for_value[val]) {
                generator_for_value[val]->generate_applicable_ops(
                    i,
                    facts,
                    applicable_ops);
            }
        }
    } else {
        const int val = facts[i].second;
        const std::unique_ptr<GeneratorBase<Entry>>& generator_for_val =
            generator_for_value[val];
        if (generator_for_val) {
            generator_for_val->generate_applicable_ops(
                i + 1,
                facts,
                applicable_ops);
        }
    }
}

template <typename Entry>
GeneratorSwitchHash<Entry>::GeneratorSwitchHash(
    int switch_var_id,
    std::unordered_map<int, std::unique_ptr<GeneratorBase<Entry>>>&&
        generator_for_value)
    : switch_var_id(switch_var_id)
    , generator_for_value(move(generator_for_value))
{
}

template <typename Entry>
void GeneratorSwitchHash<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    int val = state[switch_var_id];
    const auto& child = generator_for_value.find(val);
    if (child != generator_for_value.end()) {
        const std::unique_ptr<GeneratorBase<Entry>>& generator_for_val =
            child->second;
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchHash<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    int val = state[switch_var_id];
    const auto& child = generator_for_value.find(val);
    if (child != generator_for_value.end()) {
        const std::unique_ptr<GeneratorBase<Entry>>& generator_for_val =
            child->second;
        generator_for_val->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchHash<Entry>::generate_applicable_ops(
    unsigned i,
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    while (i < facts.size() && facts[i].first < switch_var_id) {
        ++i;
    }
    if (i == facts.size() || facts[i].first > switch_var_id) {
        for (auto it = generator_for_value.begin();
             it != generator_for_value.end();
             ++it) {
            it->second->generate_applicable_ops(i, facts, applicable_ops);
        }
    } else {
        const int val = facts[i].second;
        const auto it = generator_for_value.find(val);
        if (it != generator_for_value.end()) {
            it->second->generate_applicable_ops(i + 1, facts, applicable_ops);
        }
    }
}

template <typename Entry>
GeneratorSwitchSingle<Entry>::GeneratorSwitchSingle(
    int switch_var_id,
    int value,
    std::unique_ptr<GeneratorBase<Entry>> generator_for_value)
    : switch_var_id(switch_var_id)
    , value(value)
    , generator_for_value(move(generator_for_value))
{
}

template <typename Entry>
void GeneratorSwitchSingle<Entry>::generate_applicable_ops(
    const GlobalState& state,
    std::vector<Entry>& applicable_ops) const
{
    if (value == state[switch_var_id]) {
        generator_for_value->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchSingle<Entry>::generate_applicable_ops(
    const std::vector<int>& state,
    std::vector<Entry>& applicable_ops) const
{
    if (value == state[switch_var_id]) {
        generator_for_value->generate_applicable_ops(state, applicable_ops);
    }
}

template <typename Entry>
void GeneratorSwitchSingle<Entry>::generate_applicable_ops(
    unsigned i,
    const std::vector<std::pair<int, int>>& facts,
    std::vector<Entry>& applicable_ops) const
{
    while (i < facts.size() && facts[i].first < switch_var_id) {
        ++i;
    }
    if (i == facts.size() || facts[i].first > switch_var_id) {
        generator_for_value->generate_applicable_ops(i, facts, applicable_ops);
    } else if (facts[i].second == value) {
        generator_for_value->generate_applicable_ops(
            i + 1,
            facts,
            applicable_ops);
    }
}

template <typename Entry>
GeneratorLeafVector<Entry>::GeneratorLeafVector(
    std::vector<Entry>&& applicable_operators)
    : applicable_operators(move(applicable_operators))
{
}

template <typename Entry>
void GeneratorLeafVector<Entry>::generate_applicable_ops(
    const GlobalState&,
    std::vector<Entry>& applicable_ops) const
{
    // See above for the reason for using push_back instead of insert.
    for (Entry id : applicable_operators) {
        applicable_ops.push_back(id);
    }
}

template <typename Entry>
void GeneratorLeafVector<Entry>::generate_applicable_ops(
    const std::vector<int>&,
    std::vector<Entry>& applicable_ops) const
{
    // See above for the reason for using push_back instead of insert.
    for (Entry id : applicable_operators) {
        applicable_ops.push_back(id);
    }
}

template <typename Entry>
void GeneratorLeafVector<Entry>::generate_applicable_ops(
    unsigned,
    const std::vector<std::pair<int, int>>&,
    std::vector<Entry>& applicable_ops) const
{
    // See above for the reason for using push_back instead of insert.
    for (Entry id : applicable_operators) {
        applicable_ops.push_back(id);
    }
}

template <typename Entry>
GeneratorLeafSingle<Entry>::GeneratorLeafSingle(Entry applicable_operator)
    : applicable_operator(applicable_operator)
{
}

template <typename Entry>
void GeneratorLeafSingle<Entry>::generate_applicable_ops(
    const GlobalState&,
    std::vector<Entry>& applicable_ops) const
{
    applicable_ops.push_back(applicable_operator);
}

template <typename Entry>
void GeneratorLeafSingle<Entry>::generate_applicable_ops(
    const std::vector<int>&,
    std::vector<Entry>& applicable_ops) const
{
    applicable_ops.push_back(applicable_operator);
}

template <typename Entry>
void GeneratorLeafSingle<Entry>::generate_applicable_ops(
    unsigned,
    const std::vector<std::pair<int, int>>&,
    std::vector<Entry>& applicable_ops) const
{
    applicable_ops.push_back(applicable_operator);
}

} // namespace successor_generator
} // namespace legacy

#endif
