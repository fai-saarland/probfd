#include "probfd/open_lists/lifo_open_list.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace open_lists {

LifoOpenList::LifoOpenList(const options::Options&)
{
}

void LifoOpenList::add_options_to_parser(options::OptionParser&)
{
}

void LifoOpenList::clear()
{
    queue_.clear();
}

unsigned LifoOpenList::size() const
{
    return queue_.size();
}

StateID LifoOpenList::pop()
{
    StateID s = queue_.back();
    queue_.pop_back();
    return s;
}

void LifoOpenList::push(const StateID& state_id)
{
    queue_.push_back(state_id);
}

static Plugin<GlobalStateOpenList> _plugin(
    "lifo_open_list",
    options::parse<GlobalStateOpenList, LifoOpenList>);

} // namespace open_lists
} // namespace probfd
