#ifndef MDPS_OPEN_LISTS_LIFO_OPEN_LIST_H
#define MDPS_OPEN_LISTS_LIFO_OPEN_LIST_H

#include "probfd/open_list.h"

#include <deque>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace open_lists {

class LifoOpenList : public GlobalStateOpenList {
public:
    LifoOpenList(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

    virtual unsigned size() const override;
    virtual void clear() override;
    virtual StateID pop() override;
    virtual void push(const StateID& state_id) override;

private:
    std::deque<StateID> queue_;
};

} // namespace open_lists
} // namespace probfd

#endif // __LIFO_OPEN_LIST_H__