#include "abstract_state.h"

#include "../../../global_state.h"
#include "../../../globals.h"

#include <cassert>
#include <limits>
#include <sstream>

namespace probabilistic {
namespace pdbs {

AbstractStateMapper::CartesianSubsetIterator::
CartesianSubsetIterator(
    std::vector<int> values,
    std::vector<int> indices,
    const std::vector<int>& multipliers,
    const std::vector<int>& domains)
    : values_(std::move(values))
    , indices_(std::move(indices))
    , multipliers_(multipliers)
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
    AbstractState state,
    std::vector<int> indices,
    const std::vector<int>& multipliers,
    const std::vector<int>& domains)
    : state_(state)
    , indices_(std::move(indices))
    , index_multipliers_(indices_.size())
    , iterations_left_(indices_.size())
    , index(indices_.size())
{
    for (size_t i = 0; i != indices_.size(); ++i) {
        const int index = indices_[i];
        index_multipliers_[i] = multipliers[index];
        iterations_left_[i] = domains[index];
    }
}

AbstractStateMapper::PartialStateIterator&
AbstractStateMapper::PartialStateIterator::operator++()
{
    assert(index >= 0);

    do {
        if (static_cast<size_t>(index) == indices_.size()) {
            --index;
            break;
        }
        
        if (iterations_left_[index] > 0) {
            state_.id += index_multipliers_[index];
            --iterations_left_[index];
            ++index;
        } else {
            --index;
        }
    } while (index >= 0);

    return *this;
}

AbstractState
AbstractStateMapper::PartialStateIterator::operator*()
{
    return state_;
}

AbstractState*
AbstractStateMapper::PartialStateIterator::operator->()
{
    return &state_;
}

bool operator==(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateEndIterator&)
{
    return a.index == -1;
}

bool operator!=(
    const AbstractStateMapper::PartialStateIterator& a,
    const AbstractStateMapper::PartialStateEndIterator&)
{
    return a.index != -1;
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

    size_ = multipliers_.back() * d;
}

unsigned
AbstractStateMapper::size() const
{
    return size_;
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
    return CartesianSubsetIterator(values, indices, multipliers_, domains_);
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

AbstractStateToString::AbstractStateToString(
    std::shared_ptr<AbstractStateMapper> state_mapper)
    : state_mapper_(std::move(state_mapper))
{
}

std::string
AbstractStateToString::operator()(AbstractState state) const
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

std::ostream&
operator<<(std::ostream& out, const AbstractState& s)
{
    out << "PDBState(" << s.id << ")";
    return out;
}
} // namespace pdbs
} // namespace probabilistic

namespace utils {
void
feed(HashState& h, const probabilistic::pdbs::AbstractState& s)
{
    feed(h, s.id);
}
} // namespace utils
