#include "downward/open_lists/best_first_open_list.h"

#include "downward/evaluator.h"
#include "downward/open_list.h"

#include "downward/utils/memory.h"

#include <cassert>
#include <deque>
#include <map>

using namespace std;

namespace downward::standard_scalar_open_list {

BestFirstOpenListFactory::BestFirstOpenListFactory(
    const shared_ptr<TaskDependentFactory<Evaluator>>& eval_factory,
    bool pref_only)
    : eval_factory(eval_factory)
    , pref_only(pref_only)
{
}

unique_ptr<StateOpenList> BestFirstOpenListFactory::create_state_open_list(
    const std::shared_ptr<AbstractTask>& task)
{
    return std::make_unique<BestFirstOpenList<StateOpenListEntry>>(
        eval_factory->create_object(task),
        pref_only);
}

unique_ptr<EdgeOpenList> BestFirstOpenListFactory::create_edge_open_list(
    const std::shared_ptr<AbstractTask>& task)
{
    return std::make_unique<BestFirstOpenList<EdgeOpenListEntry>>(
        eval_factory->create_object(task),
        pref_only);
}

} // namespace downward::standard_scalar_open_list
