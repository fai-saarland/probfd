#include "partial_state.h"

#include <algorithm>
#include <cassert>

PartialState::const_iterator::const_iterator(
    const PartialState* state,
    unsigned var)
    : state_(state)
    , var_(var)
{
}

PartialState::const_iterator::const_iterator(const PartialState* state)
    : state_(state)
    , var_(-1)
{
    this->operator++();
}

PartialState::const_iterator&
PartialState::const_iterator::operator++()
{
    while (++var_ < state_->values_->size()
           && state_->values_->at(var_) == -1) {
    }
    return *this;
}

PartialState::const_iterator
PartialState::const_iterator::operator++(int i)
{
    const_iterator result(*this);
    for (; i > 0; --i) {
        this->operator++();
    }
    return result;
}

bool
PartialState::const_iterator::operator==(const const_iterator& other) const
{
    return state_ == other.state_ && var_ == other.var_;
}

bool
PartialState::const_iterator::operator!=(const const_iterator& other) const
{
    return state_ != other.state_ || var_ != other.var_;
}

PartialState::const_iterator::reference
    PartialState::const_iterator::operator*() const
{
    assert(var_ < state_->values_->size());
    assert(state_->values_->at(var_) != -1);
    return std::pair<int, int>(var_, state_->values_->at(var_));
}

PartialState::PartialState(std::vector<int>* values)
    : values_(values)
    , delete_ptr_(false)
{
}

PartialState::PartialState(const std::vector<int>& values)
    : values_(new std::vector<int>(values))
    , delete_ptr_(true)
{
}

PartialState::PartialState(std::vector<int>&& vals)
    : values_(new std::vector<int>(std::move(vals)))
    , delete_ptr_(true)
{
}

PartialState::PartialState(unsigned vars)
    : values_(new std::vector<int>(vars, -1))
    , delete_ptr_(true)
{
}

PartialState::~PartialState()
{
    if (delete_ptr_)
        delete (values_);
}

int PartialState::operator[](int var) const { return values_->at(var); }

int& PartialState::operator[](int var) { return values_->at(var); }

bool
PartialState::is_defined(int var) const
{
    return values_->at(var) != -1;
}

PartialState::const_iterator
PartialState::begin() const
{
    return const_iterator(this);
}

PartialState::const_iterator
PartialState::end() const
{
    return const_iterator(this, values_->size());
}

void
PartialState::clear()
{
    std::fill(values_->begin(), values_->end(), -1);
}
