#include "abstract_state_mapper.h"

#include "../../../global_state.h"
#include "../../../globals.h"

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
            "std::numeric_limits<int>::max()" << std::endl;
    }
};

AbstractStateMapper::CartesianSubsetIterator::
CartesianSubsetIterator(
    std::vector<int> values,
    std::vector<int> indices,
    const std::vector<int>& domains)
    : values_(std::move(values))
    , indices_(std::move(indices))
    , domains_(domains)
    , i(indices_.size())
{
    assert(values_.size() >= indices_.size());

    for (size_t i = 0; i < indices_.size(); ++i) {
        assert(0 <= indices_[i]);
        assert(static_cast<size_t>(indices_[i]) < values_.size());
        values_[indices_[i]] = 0;
    }
}

AbstractStateMapper::CartesianSubsetIterator&
AbstractStateMapper::CartesianSubsetIterator::operator++()
{
    assert(i >= 0);

    do
    {
        if (static_cast<size_t>(i) == indices_.size()) {
            --i;
            break;
        }

        const int idx = indices_[i];
        const int next = values_[idx] + 1;

        if (next < domains_[idx]) {
            values_[idx] = next;
            ++i;
        } else {
            values_[idx] = -1;
            --i;
        }
    } while (i >= 0);

    return *this;
}

AbstractStateMapper::CartesianSubsetIterator&
AbstractStateMapper::CartesianSubsetIterator::operator--()
{
    assert(i >= 0);

    do
    {
        if (static_cast<size_t>(i) == indices_.size()) {
            --i;
            break;
        }
        
        const int idx = indices_[i];
        const int next = values_[idx] - 1;

        if (next >= 0) {
            values_[idx] = next;
            ++i;
        } else {
            values_[idx] = domains_[idx];
            --i;
        }
    } while (i >= 0);

    return *this;
}

AbstractStateMapper::CartesianSubsetIterator::reference
AbstractStateMapper::CartesianSubsetIterator::operator*()
{
    return values_;
}

AbstractStateMapper::CartesianSubsetIterator::pointer
AbstractStateMapper::CartesianSubsetIterator::operator->()
{
    return &values_;
}

bool operator==(
    const AbstractStateMapper::CartesianSubsetIterator& a,
    const AbstractStateMapper::CartesianSubsetEndIterator&)
{
    return a.i == -1;
}

bool operator!=(
    const AbstractStateMapper::CartesianSubsetIterator& a,
    const AbstractStateMapper::CartesianSubsetEndIterator&)
{
    return a.i != -1;
}


AbstractStateMapper::PartialStateIterator::
PartialStateIterator(
    AbstractState offset,
    const std::vector<int>& indices,
    const std::vector<int>& multipliers,
    const std::vector<int>& domains)
    : index_multipliers_(indices.size())
    , local_counters_(indices.size() + 1, 1)
    , argument_states_(indices.size() + 1, offset)
    , domains_(indices.size())
    , recursion_level(indices.size())
{
    for (size_t i = 0; i != indices.size(); ++i) {
        const int index = indices[i];
        index_multipliers_[i] = multipliers[index];
        domains_[i] = domains[index];
        argument_states_[i] = AbstractState(offset.id + multipliers[index]);
    }
}

AbstractStateMapper::PartialStateIterator&
AbstractStateMapper::PartialStateIterator::operator++()
{
    assert(recursion_level >= 0);

    --recursion_level;

    int max_rec_depth = static_cast<int>(domains_.size());
    while (recursion_level >= 0 && recursion_level < max_rec_depth) {
        // Recursive call
        if (local_counters_[recursion_level] < domains_[recursion_level]) {
            int this_level = recursion_level++;

            // Pass arguments
            argument_states_[recursion_level] = argument_states_[this_level];
            local_counters_[recursion_level] = 0;

            argument_states_[this_level].id += index_multipliers_[this_level];
            ++local_counters_[this_level];
        } else {
            --recursion_level;
        }
    }

    return *this;
}

AbstractState
AbstractStateMapper::PartialStateIterator::operator*()
{
    return argument_states_.back();
}

AbstractState*
AbstractStateMapper::PartialStateIterator::operator->()
{
    return &argument_states_.back();
}

bool operator==(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateEndIterator&)
{
    return a.recursion_level == -1;
}

bool operator!=(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateEndIterator&)
{
    return a.recursion_level != -1;
}


AbstractStateMapper::
AbstractStateMapper(Pattern pattern, const std::vector<int>& domains)
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

unsigned
AbstractStateMapper::size() const
{
    return multipliers_.back();
}

const Pattern&
AbstractStateMapper::get_pattern() const
{
    return vars_;
}

const std::vector<int>&
AbstractStateMapper::get_domains() const
{
    return domains_;
}

AbstractState
AbstractStateMapper::operator()(const GlobalState& state) const
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

AbstractState
AbstractStateMapper::from_values_partial(
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

AbstractState
AbstractStateMapper::from_values_partial(
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    AbstractState res(0);
    for (const auto [idx, val] : sparse_values) {
        assert(0 <= idx && idx < static_cast<int>(vars_.size()));
        assert(0 <= val && val < domains_[idx]);
        res.id += multipliers_[idx] * val;
    }
    return res;
}

AbstractState
AbstractStateMapper::from_value_partial(int idx, int val) const
{
    return AbstractState(multipliers_[idx] * val);
}

int
AbstractStateMapper::get_unique_partial_state_id(
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

std::vector<int>
AbstractStateMapper::to_values(AbstractState abstract_state) const
{
    std::vector<int> values(vars_.size(), -1);
    for (size_t i = 0; i < vars_.size(); ++i) {
        values[i] = ((int)(abstract_state.id / multipliers_[i])) % domains_[i];
    }
    return values;
}

void
AbstractStateMapper::to_values(
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
    std::vector<int> values, 
    std::vector<int> indices) const
{
    return CartesianSubsetIterator(values, indices, domains_);
}

AbstractStateMapper::CartesianSubsetEndIterator
AbstractStateMapper::cartesian_subsets_end() const 
{
    return CartesianSubsetEndIterator();
}

AbstractStateMapper::PartialStateIterator
AbstractStateMapper::partial_states_begin(
    AbstractState offset,
    std::vector<int> indices) const
{
    return PartialStateIterator(offset, indices, multipliers_, domains_);
}

AbstractStateMapper::PartialStateEndIterator
AbstractStateMapper::partial_states_end() const
{
    return PartialStateEndIterator();
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
