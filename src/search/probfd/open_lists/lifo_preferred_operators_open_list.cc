#include "probfd/open_lists/lifo_preferred_operators_open_list.h"

#include "probfd/new_state_handlers/store_preferred_operators.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace open_lists {

LifoPreferredOperatorsOpenList::LifoPreferredOperatorsOpenList(
    const options::Options& opts)
    : pref_ops_(std::dynamic_pointer_cast<
                new_state_handlers::StorePreferredOperators>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void LifoPreferredOperatorsOpenList::add_options_to_parser(
    options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

unsigned LifoPreferredOperatorsOpenList::size() const
{
    return queue0_.size() + queue1_.size();
}

void LifoPreferredOperatorsOpenList::clear()
{
    queue0_.clear();
    queue1_.clear();
}

StateID LifoPreferredOperatorsOpenList::pop()
{
    if (queue1_.empty()) {
        StateID res = queue0_.back();
        queue0_.pop_back();
        return res;
    }
    StateID res = queue1_.back();
    queue1_.pop_back();
    return res;
}

void LifoPreferredOperatorsOpenList::push(const StateID& state_id)
{
    queue1_.push_back(state_id);
}

void LifoPreferredOperatorsOpenList::push(
    const StateID& parent,
    const ProbabilisticOperator* op,
    const value_type::value_t&,
    const StateID& stateid)
{
    if (pref_ops_->get_cached_ops(parent).contains(op)) {
        queue1_.push_back(stateid);
    } else {
        queue0_.push_back(stateid);
    }
}

static Plugin<GlobalStateOpenList> _plugin(
    "lifo_preferred_operators_open_list",
    options::parse<GlobalStateOpenList, LifoPreferredOperatorsOpenList>);

} // namespace open_lists
} // namespace probfd
