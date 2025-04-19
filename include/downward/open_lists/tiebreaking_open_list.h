#ifndef OPEN_LISTS_TIEBREAKING_OPEN_LIST_H
#define OPEN_LISTS_TIEBREAKING_OPEN_LIST_H

#include "downward/evaluator.h"
#include "downward/open_list.h"

#include <deque>
#include <downward/task_dependent_factory.h>
#include <map>
#include <set>
#include <vector>

namespace downward::tiebreaking_open_list {

template <class Entry>
class TieBreakingOpenList : public OpenList<Entry> {
    using Bucket = std::deque<Entry>;

    std::map<const std::vector<int>, Bucket> buckets;
    int size;

    std::vector<std::shared_ptr<Evaluator>> evaluators;
    /*
      If allow_unsafe_pruning is true, we ignore (don't insert) states
      which the first evaluator considers a dead end, even if it is
      not a safe heuristic.
    */
    bool allow_unsafe_pruning;

    int dimension() const;

protected:
    virtual void
    do_insertion(EvaluationContext& eval_context, const Entry& entry) override;

public:
    TieBreakingOpenList(
        const std::vector<std::shared_ptr<Evaluator>>& evals,
        bool unsafe_pruning,
        bool pref_only);

    virtual Entry remove_min() override;
    virtual bool empty() const override;
    virtual void clear() override;
    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override;
    virtual bool is_dead_end(EvaluationContext& eval_context) const override;
    virtual bool
    is_reliable_dead_end(EvaluationContext& eval_context) const override;
};

template <class Entry>
TieBreakingOpenList<Entry>::TieBreakingOpenList(
    const std::vector<std::shared_ptr<Evaluator>>& evals,
    bool unsafe_pruning,
    bool pref_only)
    : OpenList<Entry>(pref_only)
    , size(0)
    , evaluators(evals)
    , allow_unsafe_pruning(unsafe_pruning)
{
}

template <class Entry>
void TieBreakingOpenList<Entry>::do_insertion(
    EvaluationContext& eval_context,
    const Entry& entry)
{
    std::vector<int> key;
    key.reserve(evaluators.size());
    for (const std::shared_ptr<Evaluator>& evaluator : evaluators)
        key.push_back(
            eval_context.get_evaluator_value_or_infinity(evaluator.get()));

    buckets[key].push_back(entry);
    ++size;
}

template <class Entry>
Entry TieBreakingOpenList<Entry>::remove_min()
{
    assert(size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    assert(!it->second.empty());
    --size;
    Entry result = it->second.front();
    it->second.pop_front();
    if (it->second.empty()) buckets.erase(it);
    return result;
}

template <class Entry>
bool TieBreakingOpenList<Entry>::empty() const
{
    return size == 0;
}

template <class Entry>
void TieBreakingOpenList<Entry>::clear()
{
    buckets.clear();
    size = 0;
}

template <class Entry>
int TieBreakingOpenList<Entry>::dimension() const
{
    return evaluators.size();
}

template <class Entry>
void TieBreakingOpenList<Entry>::get_path_dependent_evaluators(
    std::set<Evaluator*>& evals)
{
    for (const std::shared_ptr<Evaluator>& evaluator : evaluators)
        evaluator->get_path_dependent_evaluators(evals);
}

template <class Entry>
bool TieBreakingOpenList<Entry>::is_dead_end(
    EvaluationContext& eval_context) const
{
    // TODO: Properly document this behaviour.
    // If one safe heuristic detects a dead end, return true.
    if (is_reliable_dead_end(eval_context)) return true;
    // If the first heuristic detects a dead-end and we allow "unsafe
    // pruning", return true.
    if (allow_unsafe_pruning &&
        eval_context.is_evaluator_value_infinite(evaluators[0].get()))
        return true;
    // Otherwise, return true if all heuristics agree this is a dead-end.
    for (const std::shared_ptr<Evaluator>& evaluator : evaluators)
        if (!eval_context.is_evaluator_value_infinite(evaluator.get()))
            return false;
    return true;
}

template <class Entry>
bool TieBreakingOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext& eval_context) const
{
    for (const std::shared_ptr<Evaluator>& evaluator : evaluators)
        if (eval_context.is_evaluator_value_infinite(evaluator.get()) &&
            evaluator->dead_ends_are_reliable())
            return true;
    return false;
}

template <typename T>
class TieBreakingOpenListFactory : public TaskDependentFactory<OpenList<T>> {
    std::vector<std::shared_ptr<Evaluator>> evals;
    bool unsafe_pruning;
    bool pref_only;

public:
    TieBreakingOpenListFactory(
        const std::vector<std::shared_ptr<Evaluator>>& evals,
        bool unsafe_pruning,
        bool pref_only)
        : evals(evals)
        , unsafe_pruning(unsafe_pruning)
        , pref_only(pref_only)
    {
    }

    std::unique_ptr<OpenList<T>>
    create_object(const std::shared_ptr<AbstractTask>&) override
    {
        return std::make_unique<TieBreakingOpenList<T>>(
            evals,
            unsafe_pruning,
            pref_only);
    }
};
} // namespace downward::tiebreaking_open_list

#endif
