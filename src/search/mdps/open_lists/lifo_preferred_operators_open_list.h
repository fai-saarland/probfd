#pragma once

#include "../open_list.h"

#include <deque>
#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace new_state_handlers {
class StorePreferredOperators;
}

namespace open_lists {

class LifoPreferredOperatorsOpenList : public GlobalStateOpenList {
public:
    LifoPreferredOperatorsOpenList(const options::Options&);
    static void add_options_to_parser(options::OptionParser&);

    virtual unsigned size() const override;
    virtual void clear() override;
    virtual StateID pop() override;
    virtual void push(const StateID& state_id) override;
    virtual void push(
        const StateID& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const StateID& state_id) override;

private:
    std::shared_ptr<new_state_handlers::StorePreferredOperators> pref_ops_;
    std::deque<StateID> queue0_;
    std::deque<StateID> queue1_;
};

} // namespace open_lists
} // namespace probabilistic
