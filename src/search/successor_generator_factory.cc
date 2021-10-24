#ifdef SUCCESSOR_GENERATOR_FACTORY_H

#include "global_operator.h"
#include "successor_generator_internals.h"
#include "utils/collections.h"
#include "utils/memory.h"

#include <algorithm>
#include <cassert>

/*
  The key ideas of the construction algorithm are as follows.

  Initially, we sort the preconditions of the operators
  lexicographically.

  We then group the operators by the *first variable* to be tested,
  forming a group for each variable and possibly a special group for
  operators with no precondition. (This special group forms the base
  case of the recursion, leading to a leaf node in the successor
  generator.) Each group forms a contiguous subrange of the overall
  operator sequence.

  We then further group each subsequence (except for the special one)
  by the *value* that the given variable is tested against, again
  obtaining contiguous subranges of the original operator sequence.

  For each of these subranges, we "pop" the first condition and then
  recursively apply the same construction algorithm to compute a child
  successor generator for this subset of operators.

  Successor generators for different values of the same variable are
  then combined into a switch node, and the generated switch nodes for
  different variables are combined into a fork node.

  The important property of lexicographic sorting that we exploit here
  is that if the original sequence is sorted, then all subsequences we
  must consider recursively are also sorted. Crucially, this remains
  true when we pop the first condition, because this popping always
  happens within a subsequence where all operators have the *same*
  first condition.

  To make the implementation more efficient, we do not physically pop
  conditions but only keep track of how many conditions have been
  dealt with so far, which is simply the recursion depth of the
  "construct_recursive" function.

  Because we only consider contiguous subranges of the operator
  sequence and never need to modify any of the data describing the
  operators, we can simply keep track of the current operator sequence
  by a begin and end index into the overall operator sequence.
*/

namespace successor_generator {

template <typename Entry>
using GeneratorPtr = std::unique_ptr<GeneratorBase<Entry> >;

template <typename Entry>
using ValuesAndGenerators = std::vector<std::pair<int, GeneratorPtr<Entry> > >;

using SuccessorGeneratorFactoryKey = std::pair<int, int>;

template <typename Entry>
SuccessorGeneratorFactoryInfo<Entry>::SuccessorGeneratorFactoryInfo(
    Entry op,
    std::vector<SuccessorGeneratorFactoryKey> precondition)
    : op(op)
    , precondition(move(precondition))
{
}

template <typename Entry>
bool
SuccessorGeneratorFactoryInfo<Entry>::operator<(
    const SuccessorGeneratorFactoryInfo<Entry>& other) const
{
    return precondition < other.precondition;
}

template <typename Entry>
Entry
SuccessorGeneratorFactoryInfo<Entry>::get() const
{
    return op;
}

// Returns -1 as a past-the-end sentinel.
template <typename Entry>
int
SuccessorGeneratorFactoryInfo<Entry>::first(int depth) const
{
    if (depth == static_cast<int>(precondition.size())) {
        return -1;
    } else {
        return precondition[depth].first;
    }
}

template <typename Entry>
int
SuccessorGeneratorFactoryInfo<Entry>::second(int depth) const
{
    return precondition[depth].second;
}

struct Range {
    int begin;
    int end;

    Range(int begin, int end)
        : begin(begin)
        , end(end)
    {
    }

    bool empty() { return begin == end; }

    int span() { return end - begin; }
};

enum class GroupOperatorsBy { VAR, VALUE };

template <typename Entry>
class OperatorGrouper {
    const std::vector<SuccessorGeneratorFactoryInfo<Entry> >& operator_infos;
    const int depth;
    const GroupOperatorsBy group_by;
    Range range;

    const SuccessorGeneratorFactoryInfo<Entry>& get_current_op_info()
    {
        assert(!range.empty());
        return operator_infos[range.begin];
    }

    int get_current_group_key()
    {
        const SuccessorGeneratorFactoryInfo<Entry>& op_info =
            get_current_op_info();
        if (group_by == GroupOperatorsBy::VAR) {
            return op_info.first(depth);
        } else {
            assert(group_by == GroupOperatorsBy::VALUE);
            return op_info.second(depth);
        }
    }

public:
    explicit OperatorGrouper(
        const std::vector<SuccessorGeneratorFactoryInfo<Entry> >& operator_infos,
        int depth,
        GroupOperatorsBy group_by,
        Range range)
        : operator_infos(operator_infos)
        , depth(depth)
        , group_by(group_by)
        , range(range)
    {
    }

    bool done() { return range.empty(); }

    std::pair<int, Range> next()
    {
        assert(!range.empty());
        int key = get_current_group_key();
        int group_begin = range.begin;
        do {
            ++range.begin;
        } while (!range.empty() && get_current_group_key() == key);
        Range group_range(group_begin, range.begin);
        return std::make_pair(key, group_range);
    }
};

template <typename Entry>
static GeneratorPtr<Entry>
construct_fork(std::vector<GeneratorPtr<Entry> > nodes)
{
    int size = nodes.size();
    if (size == 1) {
        return move(nodes.at(0));
    } else if (size == 2) {
        return utils::make_unique_ptr<GeneratorForkBinary<Entry> >(
            move(nodes.at(0)), move(nodes.at(1)));
    } else {
        /* This general case includes the case size == 0, which can
           (only) happen for the root for tasks with no operators. */
        return utils::make_unique_ptr<GeneratorForkMulti<Entry> >(move(nodes));
    }
}

template <typename Entry>
static GeneratorPtr<Entry>
construct_leaf(
    const std::vector<SuccessorGeneratorFactoryInfo<Entry> >& infos,
    Range range)
{
    assert(!range.empty());
    std::vector<Entry> operators;
    operators.reserve(range.span());
    while (range.begin != range.end) {
        operators.emplace_back(infos[range.begin].get());
        ++range.begin;
    }

    if (operators.size() == 1) {
        return utils::make_unique_ptr<GeneratorLeafSingle<Entry> >(
            operators.front());
    } else {
        return utils::make_unique_ptr<GeneratorLeafVector<Entry> >(
            move(operators));
    }
}

template <typename Entry, typename RandomAccessIterator>
static GeneratorPtr<Entry> construct_switch(
    RandomAccessIterator domains,
    int switch_var_id,
    ValuesAndGenerators<Entry> values_and_generators)
{
    int var_domain = domains[switch_var_id];
    int num_children = values_and_generators.size();

    assert(num_children > 0);

    if (num_children == 1) {
        int value = values_and_generators[0].first;
        GeneratorPtr<Entry> generator = move(values_and_generators[0].second);
        return utils::make_unique_ptr<GeneratorSwitchSingle<Entry> >(
            switch_var_id, value, move(generator));
    }

    int vector_bytes =
        utils::estimate_vector_bytes<GeneratorPtr<Entry> >(var_domain);
    int hash_bytes =
        utils::estimate_unordered_map_bytes<int, GeneratorPtr<Entry> >(
            num_children);
    if (hash_bytes < vector_bytes) {
        std::unordered_map<int, GeneratorPtr<Entry> > generator_by_value;
        for (auto& item : values_and_generators)
            generator_by_value[item.first] = move(item.second);
        return utils::make_unique_ptr<GeneratorSwitchHash<Entry> >(
            switch_var_id, move(generator_by_value));
    } else {
        std::vector<GeneratorPtr<Entry> > generator_by_value(var_domain);
        for (auto& item : values_and_generators)
            generator_by_value[item.first] = move(item.second);
        return utils::make_unique_ptr<GeneratorSwitchVector<Entry> >(
            switch_var_id, move(generator_by_value));
    }
}

template <typename Entry, typename RandomAccessIterator>
static GeneratorPtr<Entry> construct_recursive(
    RandomAccessIterator domains,
    const std::vector<SuccessorGeneratorFactoryInfo<Entry>>& infos,
    int depth,
    Range range)
{
    std::vector<GeneratorPtr<Entry> > nodes;
    OperatorGrouper<Entry> grouper_by_var(
        infos, depth, GroupOperatorsBy::VAR, range);
    while (!grouper_by_var.done()) {
        auto var_group = grouper_by_var.next();
        int var = var_group.first;
        Range var_range = var_group.second;

        if (var == -1) {
            // Handle a group of immediately applicable operators.
            nodes.push_back(construct_leaf(infos, var_range));
        } else {
            // Handle a group of operators sharing the first precondition
            // variable.
            ValuesAndGenerators<Entry> values_and_generators;
            OperatorGrouper<Entry> grouper_by_value(
                infos, depth, GroupOperatorsBy::VALUE, var_range);
            while (!grouper_by_value.done()) {
                auto value_group = grouper_by_value.next();
                int value = value_group.first;
                Range value_range = value_group.second;

                values_and_generators.emplace_back(
                    value,
                    construct_recursive(
                        domains, infos, depth + 1, value_range));
            }

            nodes.push_back(
                construct_switch(domains, var, move(values_and_generators)));
        }
    }
    return construct_fork(move(nodes));
}

#if 0
static vector<SuccessorGeneratorFactoryKey>
build_sorted_precondition(const OperatorProxy& op)
{
    vector<SuccessorGeneratorFactoryKey> precond;
    precond.reserve(op.get_preconditions().size());
    for (FactProxy pre : op.get_preconditions())
        precond.emplace_back(pre.get_pair());
    // Preconditions must be sorted by variable.
    sort(precond.begin(), precond.end());
    return precond;
}
#endif

template <typename Entry, typename RandomAccessIterator>
GeneratorPtr<Entry> create(
    RandomAccessIterator domains,
    std::vector<SuccessorGeneratorFactoryInfo<Entry>>& infos)
{
#if 0
    OperatorsProxy operators = task_proxy.geterators();
    operator_infos.reserve(operators.size());
    for (OperatorProxy op : operators) {
        operator_infos.emplace_back(
            Entry(op.get_id()), build_sorted_precondition(op));
    }
#endif
    /* Use stable_sort rather than sort for reproducibility.
       This amounts to breaking ties by operator ID. */
    stable_sort(infos.begin(), infos.end());
    Range full_range(0, infos.size());
    return construct_recursive(domains, infos, 0, full_range);
}

} // namespace successor_generator

#endif
