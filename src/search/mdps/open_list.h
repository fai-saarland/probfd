#pragma once

#include "../global_state.h"
#include "engine_interfaces/open_list.h"
#include "engine_interfaces/state_id_map.h"
#include "probabilistic_operator.h"

namespace probabilistic {

template<>
class OpenList<const ProbabilisticOperator*> {
public:
    virtual ~OpenList() = default;
    bool empty() const;

    virtual StateID pop() = 0;
    virtual void push(const StateID& state_id) = 0;
    virtual void push(
        const StateID& parent,
        const ProbabilisticOperator* op,
        const value_type::value_t& prob,
        const StateID& state_id);

    virtual unsigned size() const = 0;
    virtual void clear() = 0;

    void set_state_id_map(StateIDMap<GlobalState>* state_id_map);

protected:
    GlobalState lookup_state(const StateID& state_id);

private:
    StateIDMap<GlobalState>* state_id_map_ = nullptr;
};

using GlobalStateOpenList = OpenList<const ProbabilisticOperator*>;

} // namespace probabilistic
