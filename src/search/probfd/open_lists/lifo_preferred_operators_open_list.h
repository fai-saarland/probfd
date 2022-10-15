#ifndef MDPS_OPEN_LISTS_LIFO_PREFERRED_OPERATORS_OPEN_LIST_H
#define MDPS_OPEN_LISTS_LIFO_PREFERRED_OPERATORS_OPEN_LIST_H

#include "probfd/open_list.h"

#include <deque>
#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {

namespace new_state_handlers {
class StorePreferredOperators;
}

/// Namespace dedicated to open list implementations.
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
} // namespace probfd

#endif // __LIFO_PREFERRED_OPERATORS_OPEN_LIST_H__