#pragma once

#include "key_based_open_list.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace new_state_handlers {
class StoreHeuristic;
}

namespace open_lists {

class GoalDistanceOpenList : public KeyBasedOpenList<int> {
public:
    explicit GoalDistanceOpenList(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual int get_key(const StateID&, const GlobalState& state) override;
    virtual int get_key(
        const StateID& state_id,
        const GlobalState& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const GlobalState& state) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

} // namespace open_lists
} // namespace probabilistic
