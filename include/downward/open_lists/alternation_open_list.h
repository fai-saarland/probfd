#ifndef OPEN_LISTS_ALTERNATION_OPEN_LIST_H
#define OPEN_LISTS_ALTERNATION_OPEN_LIST_H

#include "downward/open_list.h"
#include "downward/task_dependent_factory.h"

#include <memory>
#include <set>
#include <vector>

namespace downward::alternation_open_list {

template <class Entry>
class AlternationOpenList : public OpenList<Entry> {
    std::vector<std::unique_ptr<OpenList<Entry>>> open_lists;
    std::vector<int> priorities;

    const int boost_amount;

protected:
    virtual void
    do_insertion(EvaluationContext& eval_context, const Entry& entry) override;

public:
    AlternationOpenList(
        std::vector<std::unique_ptr<OpenList<Entry>>> open_lists,
        int boost);

    virtual Entry remove_min() override;
    virtual bool empty() const override;
    virtual void clear() override;
    virtual void boost_preferred() override;
    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override;
    virtual bool is_dead_end(EvaluationContext& eval_context) const override;
    virtual bool
    is_reliable_dead_end(EvaluationContext& eval_context) const override;
};

template <class Entry>
AlternationOpenList<Entry>::AlternationOpenList(
    std::vector<std::unique_ptr<OpenList<Entry>>> open_lists,
    int boost)
    : open_lists(std::move(open_lists))
    , boost_amount(boost)
{
    priorities.resize(open_lists.size(), 0);
}

template <class Entry>
void AlternationOpenList<Entry>::do_insertion(
    EvaluationContext& eval_context,
    const Entry& entry)
{
    for (const auto& sublist : open_lists) sublist->insert(eval_context, entry);
}

template <class Entry>
Entry AlternationOpenList<Entry>::remove_min()
{
    int best = -1;
    for (size_t i = 0; i < open_lists.size(); ++i) {
        if (!open_lists[i]->empty() &&
            (best == -1 || priorities[i] < priorities[best])) {
            best = i;
        }
    }
    assert(best != -1);
    const auto& best_list = open_lists[best];
    assert(!best_list->empty());
    ++priorities[best];
    return best_list->remove_min();
}

template <class Entry>
bool AlternationOpenList<Entry>::empty() const
{
    for (const auto& sublist : open_lists)
        if (!sublist->empty()) return false;
    return true;
}

template <class Entry>
void AlternationOpenList<Entry>::clear()
{
    for (const auto& sublist : open_lists) sublist->clear();
}

template <class Entry>
void AlternationOpenList<Entry>::boost_preferred()
{
    for (size_t i = 0; i < open_lists.size(); ++i)
        if (open_lists[i]->only_contains_preferred_entries())
            priorities[i] -= boost_amount;
}

template <class Entry>
void AlternationOpenList<Entry>::get_path_dependent_evaluators(
    std::set<Evaluator*>& evals)
{
    for (const auto& sublist : open_lists)
        sublist->get_path_dependent_evaluators(evals);
}

template <class Entry>
bool AlternationOpenList<Entry>::is_dead_end(
    EvaluationContext& eval_context) const
{
    // If one sublist is sure we have a dead end, return true.
    if (is_reliable_dead_end(eval_context)) return true;
    // Otherwise, return true if all sublists agree this is a dead-end.
    for (const auto& sublist : open_lists)
        if (!sublist->is_dead_end(eval_context)) return false;
    return true;
}

template <class Entry>
bool AlternationOpenList<Entry>::is_reliable_dead_end(
    EvaluationContext& eval_context) const
{
    for (const auto& sublist : open_lists)
        if (sublist->is_reliable_dead_end(eval_context)) return true;
    return false;
}

template <typename T>
class AlternationOpenListFactory : public TaskDependentFactory<OpenList<T>> {
    std::vector<std::shared_ptr<TaskDependentFactory<OpenList<T>>>> sublists;
    int boost;

public:
    AlternationOpenListFactory(
        const std::vector<std::shared_ptr<TaskDependentFactory<OpenList<T>>>>&
            sublists,
        int boost)
        : sublists(sublists)
        , boost(boost)
    {
    }

    std::unique_ptr<OpenList<T>>
    create_object(const SharedAbstractTask& task) override
    {
        std::vector<std::unique_ptr<OpenList<T>>> open_lists;
        open_lists.reserve(sublists.size());
        for (const auto& factory : sublists)
            open_lists.push_back(factory->create_object(task));

        return std::make_unique<AlternationOpenList<T>>(
            std::move(open_lists),
            boost);
    }
};
} // namespace downward::alternation_open_list

#endif
