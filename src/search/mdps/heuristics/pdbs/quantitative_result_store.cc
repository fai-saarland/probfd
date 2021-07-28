#include "quantitative_result_store.h"

#include <cassert>

namespace probabilistic {
namespace pdbs {

QuantitativeResultStore::QuantitativeResultStore()
{
}

bool QuantitativeResultStore::has_value(const AbstractState& x) const
{
    return values_.find(x.id) != values_.end();
}

value_type::value_t QuantitativeResultStore::get(const AbstractState& x) const
{
    auto it = values_.find(x.id);
    assert(it != values_.end());
    return it->second;
}

value_type::value_t&
QuantitativeResultStore::operator[](const AbstractState& state)
{
    auto x = values_.insert(std::make_pair(state.id, value_type::zero));
    return x.first->second;
}

value_type::value_t& QuantitativeResultStore::operator[](const StateID& id)
{
    auto x = values_.insert(std::make_pair(id, value_type::zero));
    return x.first->second;
}

std::optional<value_type::value_t>
QuantitativeResultStore::get_optional(const AbstractState& x) const
{
    auto it = values_.find(x.id);
    if (it == values_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void QuantitativeResultStore::set(
    const AbstractState& state,
    value_type::value_t value)
{
    this->operator[](state) = value;
}

} // namespace pdbs
} // namespace probabilistic
