#pragma once

#include "../new_state_handler.h"
#include "../storage/per_state_storage.h"

#include <memory>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace new_state_handlers {

class StoreHeuristic : public NewGlobalStateHandler {
public:
    explicit StoreHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    int get_cached_h_value(const StateID& s);

    virtual void touch(const GlobalState& s) override;
    virtual void touch_goal(const GlobalState& s) override;
    virtual void touch_dead_end(const GlobalState& s) override;

private:
    const bool fetch_only_;
    std::shared_ptr<Heuristic> heuristic_;
    storage::PerStateStorage<int> hstore_;
};

} // namespace new_state_handlers
} // namespace probabilistic
