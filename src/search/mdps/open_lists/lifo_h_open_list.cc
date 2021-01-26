#include "lifo_h_open_list.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_heuristic.h"

#include <algorithm>

namespace probabilistic {
namespace open_lists {

LifoHOpenList::LifoHOpenList(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
        opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
    , exp_(StateID::undefined)
{
}

void
LifoHOpenList::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

unsigned
LifoHOpenList::size() const
{
    return queue_.size() + temp_.size();
}

void
LifoHOpenList::clear()
{
    temp_.clear();
    queue_.clear();
}

StateID
LifoHOpenList::pop()
{
    populate();
    StateID res = queue_.back();
    queue_.pop_back();
    return res;
}

void
LifoHOpenList::push(const StateID& state_id)
{
    queue_.push_back(state_id);
}

void
LifoHOpenList::push(
    const StateID& parent,
    const ProbabilisticOperator*,
    const value_type::value_t&,
    const StateID& state_id)
{
    if (parent != exp_) {
        populate();
        exp_ = parent;
    }
    temp_.emplace_back(heuristic_->get_cached_h_value(state_id), state_id);
}

void
LifoHOpenList::populate()
{
    if (temp_.empty()) {
        return;
    }
    std::sort(temp_.begin(), temp_.end(), [](const auto& x, const auto& y) {
        return x.first < y.first || (x.first == y.first && x.second > y.second);
    });
    temp_.erase(std::unique(temp_.begin(), temp_.end()), temp_.end());
    const auto* ptr = &temp_[temp_.size() - 1];
    for (int i = temp_.size(); i > 0; --i, --ptr) {
        queue_.push_back(ptr->second);
    }
    temp_.clear();
}

static Plugin<GlobalStateOpenList> _plugin(
    "lifo_h_open_list",
    options::parse<GlobalStateOpenList, LifoHOpenList>);

} // namespace open_lists
} // namespace probabilistic
