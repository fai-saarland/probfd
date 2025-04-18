#include "downward/open_lists/alternation_open_list.h"

#include "downward/open_list.h"

#include "downward/utils/memory.h"
#include "downward/utils/system.h"

#include <cassert>
#include <memory>
#include <vector>

using namespace std;
using downward::utils::ExitCode;

namespace downward::alternation_open_list {

AlternationOpenListFactory::AlternationOpenListFactory(
    const vector<shared_ptr<OpenListFactory>>& sublists,
    int boost)
    : sublists(sublists)
    , boost(boost)
{
}

unique_ptr<StateOpenList> AlternationOpenListFactory::create_state_open_list(
    const std::shared_ptr<AbstractTask>& task)
{
    vector<unique_ptr<OpenList<StateOpenListEntry>>> open_lists;
    open_lists.reserve(sublists.size());
    for (const auto& factory : sublists)
        open_lists.push_back(
            factory->create_open_list<StateOpenListEntry>(task));

    return std::make_unique<AlternationOpenList<StateOpenListEntry>>(
        std::move(open_lists),
        boost);
}

unique_ptr<EdgeOpenList> AlternationOpenListFactory::create_edge_open_list(
    const std::shared_ptr<AbstractTask>& task)
{
    vector<unique_ptr<OpenList<EdgeOpenListEntry>>> open_lists;
    open_lists.reserve(sublists.size());
    for (const auto& factory : sublists)
        open_lists.push_back(
            factory->create_open_list<EdgeOpenListEntry>(task));

    return std::make_unique<AlternationOpenList<EdgeOpenListEntry>>(
        std::move(open_lists),
        boost);
}

} // namespace downward::alternation_open_list
