#pragma once

#include "open_list.h"

#include <deque>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace open_lists {

class FifoOpenList : public GlobalStateOpenList {
public:
    FifoOpenList(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

    virtual unsigned size() const override;
    virtual void clear() override;
    virtual StateID pop() override;
    virtual void push(const StateID& state_id, const GlobalState& state) override;

private:
    std::deque<StateID> queue_;
};

} // namespace open_lists
} // namespace probabilistic
