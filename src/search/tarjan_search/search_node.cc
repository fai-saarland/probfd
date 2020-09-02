#include "search_node.h"

#include <cassert>

namespace tarjan_search {

SearchNode::SearchNode()
    : info_(nullptr)
    , state_id_(StateID::no_state)
{
}

SearchNode::SearchNode(SearchNodeInfo* info, StateID state_id)
    : info_(info)
    , state_id_(state_id)
{
}

bool
SearchNode::is_new() const
{
    return info_->h == SearchNodeInfo::NEW;
}

bool
SearchNode::is_closed() const
{
    return info_->index != SearchNodeInfo::UNDEFINED;
}

bool
SearchNode::is_onstack() const
{
    return info_->onstack;
}

bool
SearchNode::is_dead_end() const
{
    return info_->h == SearchNodeInfo::DEAD_END
        || info_->h == SearchNodeInfo::RECOGNIZED_DEAD_END;
}

bool
SearchNode::is_recognized_dead_end() const
{
    return info_->h == SearchNodeInfo::RECOGNIZED_DEAD_END;
}

unsigned
SearchNode::get_index() const
{
    return info_->index;
}

unsigned
SearchNode::get_lowlink() const
{
    return info_->lowlink;
}

int
SearchNode::get_h() const
{
    return info_->h;
}

StateID
SearchNode::get_state_id() const
{
    return state_id_;
}

StateID
SearchNode::get_parent_state_id() const
{
    return info_->parent;
}

const GlobalOperator*
SearchNode::get_operator() const
{
    return info_->op;
}

void
SearchNode::mark_dead_end()
{
    assert(info_->h != SearchNodeInfo::RECOGNIZED_DEAD_END);
    info_->h = SearchNodeInfo::DEAD_END;
}

void
SearchNode::mark_recognized_dead_end()
{
    info_->h = SearchNodeInfo::RECOGNIZED_DEAD_END;
}

void
SearchNode::popped_from_stack()
{
    info_->onstack = 0;
}

void
SearchNode::close(unsigned index)
{
    assert(
        info_->index == info_->lowlink
        && info_->index == SearchNodeInfo::UNDEFINED);
    info_->onstack = 1;
    info_->index = index;
    info_->lowlink = index;
}

void
SearchNode::open_initial_state(int h)
{
    info_->h = h;
}

bool
SearchNode::update_h(int h)
{
    if (h > info_->h) {
        info_->h = h;
        return true;
    }
    return false;
}

void
SearchNode::open(const SearchNode& parent, const GlobalOperator* op, int h)
{
    info_->parent = parent.get_state_id();
    info_->op = op;
    info_->h = h;
}

bool
SearchNode::update_lowlink(unsigned index)
{
    if (index != SearchNodeInfo::UNDEFINED && index < info_->lowlink) {
        info_->lowlink = index;
        return true;
    }
    return false;
}

} // namespace tarjan_search
