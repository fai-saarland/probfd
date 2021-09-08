#include "qualitative_result_store.h"

#include <cassert>

namespace probabilistic {
namespace pdbs {

QualitativeResultStore::assignable_bool_t::assignable_bool_t(
    const AbstractState& s,
    QualitativeResultStore* store)
    : state_(s)
    , ref_(store)
{
}

QualitativeResultStore::QualitativeResultStore(
    bool is_negated,
    std::unordered_set<AbstractState> states)
    : is_negated_(is_negated)
    , states_(std::move(states))
{
}

QualitativeResultStore::assignable_bool_t&
QualitativeResultStore::assignable_bool_t::operator=(bool value)
{
    ref_->set(state_, value);
    return *this;
}

QualitativeResultStore::assignable_bool_t::operator bool() const
{
    return ref_->get(state_);
}

void QualitativeResultStore::negate_all()
{
    is_negated_ = !is_negated_;
}

void QualitativeResultStore::clear()
{
    is_negated_ = false;
    states_.clear();
}

void QualitativeResultStore::set(const AbstractState& s, bool value)
{
    value = is_negated_ ? !value : value;
    if (value) {
        states_.insert(s);
    } else {
        auto it = states_.find(s);
        if (it != states_.end()) {
            states_.erase(it);
        }
    }
}

bool QualitativeResultStore::get(const AbstractState& s) const
{
    return states_.find(s) != states_.end() ? !is_negated_ : is_negated_;
}

QualitativeResultStore::assignable_bool_t
QualitativeResultStore::operator[](const AbstractState& s)
{
    return assignable_bool_t(s, this);
}

bool QualitativeResultStore::operator[](const AbstractState& s) const
{
    return get(s);
}

} // namespace pdbs
} // namespace probabilistic
