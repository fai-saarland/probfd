#pragma once

#include "../state_id.h"

class GlobalOperator;

namespace tarjan_search {

struct SearchNodeInfo {
    static int NEW;
    static int DEAD_END;
    static int RECOGNIZED_DEAD_END;
    static unsigned UNDEFINED;
    int h : 31;
    unsigned onstack : 1;
    unsigned index;
    unsigned lowlink;
    StateID parent;
    const GlobalOperator* op;
    SearchNodeInfo()
        : h(NEW)
        , onstack(0)
        , index(UNDEFINED)
        , lowlink(UNDEFINED)
        , parent(StateID::no_state)
        , op(nullptr)
    {
    }
};

} // namespace tarjan_search
