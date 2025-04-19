#ifndef OPEN_LISTS_BEST_FIRST_OPEN_LIST_H
#define OPEN_LISTS_BEST_FIRST_OPEN_LIST_H

#include "downward/open_list.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

#include <cassert>
#include <deque>
#include <map>
#include <memory>
#include <set>

/*
  Open list indexed by a single int, using FIFO tie-breaking.

  Implemented as a map from int to deques.
*/

namespace downward::standard_scalar_open_list {

template <class Entry>
class BestFirstOpenList : public OpenList<Entry> {
    typedef std::deque<Entry> Bucket;

    std::map<int, Bucket> buckets;
    int size;

    std::shared_ptr<Evaluator> evaluator;

protected:
    virtual void
    do_insertion(EvaluationContext& eval_context, const Entry& entry) override;

public:
    BestFirstOpenList(
        const std::shared_ptr<Evaluator>& eval,
        bool preferred_only);

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
BestFirstOpenList<Entry>::BestFirstOpenList(
    const std::shared_ptr<Evaluator>& evaluator,
    bool preferred_only)
    : OpenList<Entry>(preferred_only)
    , size(0)
    , evaluator(evaluator)
{
}

template <class Entry>
void BestFirstOpenList<Entry>::do_insertion(
    EvaluationContext& eval_context,
    const Entry& entry)
{
    int key = eval_context.get_evaluator_value(evaluator.get());
    buckets[key].push_back(entry);
    ++size;
}

template <class Entry>
Entry BestFirstOpenList<Entry>::remove_min()
{
    assert(size > 0);
    auto it = buckets.begin();
    assert(it != buckets.end());
    Bucket& bucket = it->second;
    assert(!bucket.empty());
    Entry result = bucket.front();
    bucket.pop_front();
    if (bucket.empty()) buckets.erase(it);
    --size;
    return result;
}

template <class Entry>
bool BestFirstOpenList<Entry>::empty() const
{
    return size == 0;
}

template <class Entry>
void BestFirstOpenList<Entry>::clear()
{
    buckets.clear();
    size = 0;
}

template <class Entry>
void BestFirstOpenList<Entry>::get_path_dependent_evaluators(
    std::set<Evaluator*>& evals)
{
    evaluator->get_path_dependent_evaluators(evals);
}

template <class Entry>
bool BestFirstOpenList<Entry>::is_dead_end(
    EvaluationContext& eval_context) const
{
    return eval_context.is_evaluator_value_infinite(evaluator.get());
}

template <class Entry>
bool BestFirstOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext& eval_context) const
{
    return is_dead_end(eval_context) && evaluator->dead_ends_are_reliable();
}

template <typename T>
class BestFirstOpenListFactory : public TaskDependentFactory<OpenList<T>> {
    std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory;
    bool pref_only;

public:
    BestFirstOpenListFactory(
        const std::shared_ptr<TaskDependentFactory<Evaluator>>& eval_factory,
        bool pref_only)
        : eval_factory(std::move(eval_factory))
        , pref_only(pref_only)
    {
    }

    std::unique_ptr<OpenList<T>>
    create_object(const std::shared_ptr<AbstractTask>& task) override
    {
        return std::make_unique<BestFirstOpenList<T>>(
            eval_factory->create_object(task),
            pref_only);
    }
};
} // namespace downward::standard_scalar_open_list

#endif
