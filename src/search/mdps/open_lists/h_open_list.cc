#include "h_open_list.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_heuristic.h"

namespace probabilistic {
namespace open_lists {

GoalDistanceOpenList::GoalDistanceOpenList(const options::Options& opts)
    : KeyBasedOpenList<int>()
    , heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void
GoalDistanceOpenList::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

int
GoalDistanceOpenList::get_key(const StateID&, const GlobalState& state)
{
    return heuristic_->get_cached_h_value(state);
}

int
GoalDistanceOpenList::get_key(
    const StateID&,
    const GlobalState&,
    const ProbabilisticOperator*,
    const value_type::value_t&,
    const GlobalState& state)
{
    return heuristic_->get_cached_h_value(state);
}

static Plugin<GlobalStateOpenList> _plugin(
    "h_open_list",
    options::parse<GlobalStateOpenList, GoalDistanceOpenList>);

} // namespace open_lists
} // namespace probabilistic
