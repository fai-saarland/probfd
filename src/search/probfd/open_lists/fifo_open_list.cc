#include "fifo_open_list.h"

#include "../../option_parser.h"
#include "../../plugin.h"

namespace probfd {
namespace open_lists {

FifoOpenList::FifoOpenList(const options::Options&)
{
}

void FifoOpenList::add_options_to_parser(options::OptionParser&)
{
}

void FifoOpenList::clear()
{
    queue_.clear();
}

unsigned FifoOpenList::size() const
{
    return queue_.size();
}

StateID FifoOpenList::pop()
{
    StateID s = queue_.front();
    queue_.pop_front();
    return s;
}

void FifoOpenList::push(const StateID& state_id)
{
    queue_.push_back(state_id);
}

static Plugin<GlobalStateOpenList> _plugin(
    "fifo_open_list",
    options::parse<GlobalStateOpenList, FifoOpenList>);

} // namespace open_lists
} // namespace probfd
