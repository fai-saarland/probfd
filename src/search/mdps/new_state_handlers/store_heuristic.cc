#include "store_heuristic.h"

#include "../../global_state.h"
#include "../../heuristic.h"
#include "../../option_parser.h"
#include "../../plugin.h"

namespace probabilistic {
namespace new_state_handlers {

StoreHeuristic::StoreHeuristic(const options::Options& opts)
    : fetch_only_(opts.get<bool>("fetch_only"))
    , heuristic_(opts.get<std::shared_ptr<Heuristic>>("heuristic"))
    , hstore_(-2)
{
}

void
StoreHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    parser.add_option<bool>("fetch_only", "", "false");
}

int
StoreHeuristic::get_cached_h_value(const GlobalState& s)
{
    const int res = hstore_[s.get_id()];
    assert(res != -2);
    return res;
}

void
StoreHeuristic::touch_goal(const GlobalState& s)
{
    hstore_[s.get_id()] = 0;
}

void
StoreHeuristic::touch_dead_end(const GlobalState& s)
{
    hstore_[s.get_id()] = -1;
}

void
StoreHeuristic::touch(const GlobalState& s)
{
    if (!fetch_only_) {
        heuristic_->evaluate(s);
    }
    hstore_[s.get_id()] = heuristic_->get_value();
}

static Plugin<NewGlobalStateHandler> _plugin(
    "store_heuristic",
    options::parse<NewGlobalStateHandler, StoreHeuristic>);
} // namespace new_state_handlers
} // namespace probabilistic
