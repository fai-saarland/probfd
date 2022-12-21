#ifndef MDPS_NEW_STATE_HANDLERS_STORE_HEURISTIC_H
#define MDPS_NEW_STATE_HANDLERS_STORE_HEURISTIC_H

#include "probfd/new_state_handler.h"

#include "probfd/storage/per_state_storage.h"

#include <memory>

namespace legacy {
class Heuristic;
}

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace new_state_handlers {

class StoreHeuristic : public NewGlobalStateHandler {
public:
    explicit StoreHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    int get_cached_h_value(const StateID& s);

    virtual void touch(const legacy::GlobalState& s) override;
    virtual void touch_goal(const legacy::GlobalState& s) override;
    virtual void touch_dead_end(const legacy::GlobalState& s) override;

private:
    const bool fetch_only_;
    std::shared_ptr<legacy::Heuristic> heuristic_;
    storage::PerStateStorage<int> hstore_;
};

} // namespace new_state_handlers
} // namespace probfd

#endif // __STORE_HEURISTIC_H__