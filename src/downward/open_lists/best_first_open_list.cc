#include "downward/open_lists/best_first_open_list.h"

#include "downward/evaluator.h"
#include "downward/open_list.h"

#include "downward/utils/memory.h"

#include <cassert>
#include <deque>
#include <map>

using namespace std;

namespace downward::standard_scalar_open_list {
template <class Entry>
class BestFirstOpenList : public OpenList<Entry> {
    typedef deque<Entry> Bucket;

    map<int, Bucket> buckets;
    int size;

    shared_ptr<Evaluator> evaluator;

protected:
    virtual void
    do_insertion(EvaluationContext& eval_context, const Entry& entry) override;

public:
    BestFirstOpenList(const shared_ptr<Evaluator>& eval, bool preferred_only);

    virtual Entry remove_min() override;
    virtual bool empty() const override;
    virtual void clear() override;
    virtual void get_path_dependent_evaluators(set<Evaluator*>& evals) override;
    virtual bool is_dead_end(EvaluationContext& eval_context) const override;
    virtual bool
    is_reliable_dead_end(EvaluationContext& eval_context) const override;
};

template <class Entry>
BestFirstOpenList<Entry>::BestFirstOpenList(
    const shared_ptr<Evaluator>& evaluator,
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
    set<Evaluator*>& evals)
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

BestFirstOpenListFactory::BestFirstOpenListFactory(
    const shared_ptr<Evaluator>& eval,
    bool pref_only)
    : eval(eval)
    , pref_only(pref_only)
{
}

unique_ptr<StateOpenList> BestFirstOpenListFactory::create_state_open_list()
{
    return std::make_unique<BestFirstOpenList<StateOpenListEntry>>(
        eval,
        pref_only);
}

unique_ptr<EdgeOpenList> BestFirstOpenListFactory::create_edge_open_list()
{
    return std::make_unique<BestFirstOpenList<EdgeOpenListEntry>>(
        eval,
        pref_only);
}

} // namespace standard_scalar_open_list
