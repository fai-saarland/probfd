#include "abstract_state_mapper.h"

#include "../../../global_state.h"
#include "../../../globals.h"

#include "../../../utils/collections.h"
#include "../../../utils/exceptions.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <sstream>

namespace probabilistic {
namespace pdbs {

struct PatternTooLargeException : utils::Exception {
    void print() const override
    {
        std::cerr << "Construction of PDB would exceed "
                     "std::numeric_limits<int>::max()"
                  << std::endl;
    }
};

AbstractStateMapper::CartesianSubsetIterator::CartesianSubsetIterator(
    std::vector<std::pair<int, int>> partial_state,
    const std::vector<int>& domains)
    : partial_state_(std::move(partial_state))
    , domains_(domains)
    , done(false)
{
}

AbstractStateMapper::CartesianSubsetIterator&
AbstractStateMapper::CartesianSubsetIterator::operator++()
{
    for (int i = partial_state_.size() - 1; i >= 0; --i) {
        auto& [var, val] = partial_state_[i];
        const int next = val + 1;

        if (next < domains_[var]) {
            val = next;
            return *this;
        }

        val = 0;
    }

    done = true;

    return *this;
}

AbstractStateMapper::CartesianSubsetIterator&
AbstractStateMapper::CartesianSubsetIterator::operator--()
{
    for (int i = partial_state_.size() - 1; i >= 0; --i) {
        auto& [var, val] = partial_state_[i];
        const int next = val - 1;

        if (next >= 0) {
            val = next;
            return *this;
        }

        val = domains_[var] - 1;
    }

    done = true;

    return *this;
}

AbstractStateMapper::CartesianSubsetIterator::reference
AbstractStateMapper::CartesianSubsetIterator::operator*()
{
    return partial_state_;
}

AbstractStateMapper::CartesianSubsetIterator::pointer
AbstractStateMapper::CartesianSubsetIterator::operator->()
{
    return &partial_state_;
}

bool operator==(
    const AbstractStateMapper::CartesianSubsetIterator& a,
    const AbstractStateMapper::CartesianSubsetSentinel&)
{
    return a.done;
}

bool operator!=(
    const AbstractStateMapper::CartesianSubsetIterator& a,
    const AbstractStateMapper::CartesianSubsetSentinel&)
{
    return !a.done;
}

AbstractStateMapper::PartialStateIterator::PartialStateIterator(
    AbstractState offset,
    const std::vector<int>& indices,
    const std::vector<int>& multipliers,
    const std::vector<int>& domains)
    : values_(indices.size(), 0)
    , domains_(indices.size())
    , multipliers_(indices.size())
    , state_(offset)
    , done(false)
{
    for (size_t i = 0; i != indices.size(); ++i) {
        const int index = indices[i];
        multipliers_[i] = multipliers[index];
        domains_[i] = domains[index];
    }
}

AbstractStateMapper::PartialStateIterator&
AbstractStateMapper::PartialStateIterator::operator++()
{
    for (int idx = values_.size() - 1; idx >= 0; --idx) {
        const int next = values_[idx] + 1;

        if (next < domains_[idx]) {
            values_[idx] = next;
            state_.id += multipliers_[idx];
            return *this;
        }

        state_.id -= (values_[idx] * multipliers_[idx]);
        values_[idx] = 0;
    }

    done = true;

    return *this;
}

const AbstractState& AbstractStateMapper::PartialStateIterator::operator*()
{
    return state_;
}

const AbstractState* AbstractStateMapper::PartialStateIterator::operator->()
{
    return &state_;
}

bool operator==(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateSentinel&)
{
    return a.done;
}

bool operator!=(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateSentinel&)
{
    return !a.done;
}

AbstractStateMapper::AbstractStateMapper(
    Pattern pattern,
    const std::vector<int>& domains)
    : vars_(std::move(pattern))
    , domains_(vars_.size())
    , multipliers_(vars_.size(), 1)
    , partial_multipliers_(vars_.size(), 1)
{
    assert(!vars_.empty());
    assert(std::is_sorted(vars_.begin(), vars_.end()));

    constexpr int maxint = std::numeric_limits<int>::max();

    for (unsigned i = 1; i < vars_.size(); i++) {
        assert(vars_[i - 1] < static_cast<int>(domains.size()));
        const int d = domains[vars_[i - 1]];
        domains_[i - 1] = d;

        if (partial_multipliers_[i - 1] > maxint / (d + 1)) {
            throw PatternTooLargeException();
        }

        multipliers_[i] = multipliers_[i - 1] * d;
        partial_multipliers_[i] = partial_multipliers_[i - 1] * (d + 1);
    }

    assert(vars_.back() < static_cast<int>(domains.size()));

    const int d = domains[vars_.back()];
    domains_.back() = d;

    if (multipliers_.back() > maxint / d) {
        throw PatternTooLargeException();
    }

    multipliers_.push_back(multipliers_.back() * d);
}

unsigned AbstractStateMapper::size() const
{
    return multipliers_.back();
}

const Pattern& AbstractStateMapper::get_pattern() const
{
    return vars_;
}

const std::vector<int>& AbstractStateMapper::get_domains() const
{
    return domains_;
}

AbstractState AbstractStateMapper::operator()(const GlobalState& state) const
{
    AbstractState res(0);
    for (size_t i = 0; i < vars_.size(); ++i) {
        res.id += multipliers_[i] * state[vars_[i]];
    }
    return res;
}

AbstractState
AbstractStateMapper::operator()(const std::vector<int>& state) const
{
    AbstractState res(0);
    for (size_t i = 0; i < vars_.size(); ++i) {
        res.id += multipliers_[i] * state[vars_[i]];
    }
    return res;
}

AbstractState
AbstractStateMapper::from_values(const std::vector<int>& values) const
{
    assert(values.size() == vars_.size());
    AbstractState res(0);
    for (size_t i = 0; i < vars_.size(); ++i) {
        res.id += multipliers_[i] * values[i];
    }
    return res;
}

AbstractState AbstractStateMapper::from_values_partial(
    const std::vector<int>& indices,
    const std::vector<int>& values) const
{
    assert(values.size() == vars_.size());
    AbstractState res(0);
    for (int j : indices) {
        res.id += multipliers_[j] * values[j];
    }
    return res;
}

AbstractState AbstractStateMapper::from_values_partial(
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    AbstractState res(0);
    for (const auto& [idx, val] : sparse_values) {
        assert(0 <= idx && idx < static_cast<int>(vars_.size()));
        assert(0 <= val && val < domains_[idx]);
        res.id += multipliers_[idx] * val;
    }
    return res;
}

AbstractState AbstractStateMapper::from_values_partial(
    const std::vector<int>& indices,
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    AbstractState res(0);

    auto ind_it = indices.begin();
    auto ind_end = indices.end();

    auto it = sparse_values.begin();
    auto end = sparse_values.end();

    for (; ind_it != ind_end; ++it, ++ind_it) {
        const int idx = *ind_it;

        it = std::find_if(it, end, [=](auto a) { return a.first == idx; });
        assert(it != end);

        res.id += multipliers_[idx] * it->second;
    }

    return res;
}

AbstractState AbstractStateMapper::from_value_partial(int idx, int val) const
{
    return AbstractState(multipliers_[idx] * val);
}

int AbstractStateMapper::get_unique_partial_state_id(
    const std::vector<int>& indices,
    const std::vector<int>& values) const
{
    assert(values.size() == vars_.size());
    int id = 0;
    for (int j : indices) {
        id += partial_multipliers_[j] * (values[j] + 1);
    }
    return id;
}

int AbstractStateMapper::get_unique_partial_state_id(
    const std::vector<std::pair<int, int>>& pstate) const
{
    int id = 0;
    for (const auto& [var, val] : pstate) {
        id += partial_multipliers_[var] * (val + 1);
    }
    return id;
}

std::vector<int>
AbstractStateMapper::to_values(AbstractState abstract_state) const
{
    std::vector<int> values(vars_.size(), -1);
    for (size_t i = 0; i < vars_.size(); ++i) {
        values[i] = ((int)(abstract_state.id / multipliers_[i])) % domains_[i];
    }
    return values;
}

void AbstractStateMapper::to_values(
    AbstractState abstract_state,
    std::vector<int>& values) const
{
    values.resize(vars_.size());
    for (size_t i = 0; i < vars_.size(); ++i) {
        values[i] = ((int)(abstract_state.id / multipliers_[i])) % domains_[i];
    }
}

AbstractStateMapper::CartesianSubsetIterator
AbstractStateMapper::cartesian_subsets_begin(
    std::vector<std::pair<int, int>> partial_state) const
{
    return CartesianSubsetIterator(std::move(partial_state), domains_);
}

AbstractStateMapper::CartesianSubsetSentinel
AbstractStateMapper::cartesian_subsets_end() const
{
    return CartesianSubsetSentinel();
}

utils::RangeProxy<
    AbstractStateMapper::CartesianSubsetIterator,
    AbstractStateMapper::CartesianSubsetSentinel>
AbstractStateMapper::cartesian_subsets(
    std::vector<std::pair<int, int>> partial_state) const
{
    return utils::RangeProxy<CartesianSubsetIterator, CartesianSubsetSentinel>(
        cartesian_subsets_begin(std::move(partial_state)),
        cartesian_subsets_end());
}

AbstractStateMapper::PartialStateIterator
AbstractStateMapper::partial_states_begin(
    AbstractState offset,
    std::vector<int> indices) const
{
    return PartialStateIterator(offset, indices, multipliers_, domains_);
}

AbstractStateMapper::PartialStateSentinel
AbstractStateMapper::partial_states_end() const
{
    return PartialStateSentinel();
}

utils::RangeProxy<
    AbstractStateMapper::PartialStateIterator,
    AbstractStateMapper::PartialStateSentinel>
AbstractStateMapper::partial_states(
    AbstractState offset,
    std::vector<int> indices) const
{
    return utils::RangeProxy<PartialStateIterator, PartialStateSentinel>(
        partial_states_begin(offset, indices),
        partial_states_end());
}

int AbstractStateMapper::get_multiplier(int var) const
{
    return get_multiplier_raw(get_index(var));
}

int AbstractStateMapper::get_multiplier_raw(int idx) const
{
    return multipliers_[idx];
}

int AbstractStateMapper::get_domain_size(int var) const
{
    return get_domain_size_raw(get_index(var));
}

int AbstractStateMapper::get_domain_size_raw(int idx) const
{
    return domains_[idx];
}

int AbstractStateMapper::get_index(int var) const
{
    auto it = std::find(vars_.begin(), vars_.end(), var);
    return it != vars_.end() ? std::distance(vars_.begin(), it) : -1;
}

AbstractStateToString::AbstractStateToString(
    std::shared_ptr<AbstractStateMapper> state_mapper)
    : state_mapper_(std::move(state_mapper))
{
}

std::string AbstractStateToString::operator()(AbstractState state) const
{
    std::ostringstream out;
    std::vector<int> values = state_mapper_->to_values(state);
    out << "#" << state.id << ": ";
    for (unsigned i = 0; i < values.size(); i++) {
        const int var = state_mapper_->get_pattern()[i];
        out << (i > 0 ? ", " : "") << ::g_fact_names[var][values[i]];
    }
    return out.str();
}

} // namespace pdbs
} // namespace probabilistic
