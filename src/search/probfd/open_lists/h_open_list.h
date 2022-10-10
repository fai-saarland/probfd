#ifndef MDPS_OPEN_LISTS_H_OPEN_LIST_H
#define MDPS_OPEN_LISTS_H_OPEN_LIST_H

#include "key_based_open_list.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

namespace new_state_handlers {
class StoreHeuristic;
}

namespace open_lists {

class GoalDistanceOpenList : public KeyBasedOpenList<int> {
public:
    explicit GoalDistanceOpenList(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual int get_key(const StateID&) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
};

} // namespace open_lists
} // namespace probfd

#endif // __H_OPEN_LIST_H__