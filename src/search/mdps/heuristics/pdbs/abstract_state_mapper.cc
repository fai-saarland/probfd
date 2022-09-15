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

AbstractStateMapper::PartialAssignmentIterator::PartialAssignmentIterator(
    std::vector<std::pair<int, int>> partial_state,
    const std::vector<VariableInfo>& var_infos)
    : partial_state_(std::move(partial_state))
    , var_infos_(var_infos)
    , done(false)
{
}

AbstractStateMapper::PartialAssignmentIterator&
AbstractStateMapper::PartialAssignmentIterator::operator++()
{
    for (int i = partial_state_.size() - 1; i >= 0; --i) {
        auto& [var, val] = partial_state_[i];
        const int next = val + 1;

        if (next < var_infos_[var].domain) {
            val = next;
            return *this;
        }

        val = 0;
    }

    done = true;

    return *this;
}

AbstractStateMapper::PartialAssignmentIterator&
AbstractStateMapper::PartialAssignmentIterator::operator--()
{
    for (int i = partial_state_.size() - 1; i >= 0; --i) {
        auto& [var, val] = partial_state_[i];
        const int next = val - 1;

        if (next >= 0) {
            val = next;
            return *this;
        }

        val = var_infos_[var].domain - 1;
    }

    done = true;

    return *this;
}

AbstractStateMapper::PartialAssignmentIterator::reference
AbstractStateMapper::PartialAssignmentIterator::operator*()
{
    return partial_state_;
}

AbstractStateMapper::PartialAssignmentIterator::pointer
AbstractStateMapper::PartialAssignmentIterator::operator->()
{
    return &partial_state_;
}

bool operator==(
    const AbstractStateMapper::PartialAssignmentIterator& a,
    const utils::default_sentinel_t&)
{
    return a.done;
}

bool operator!=(
    const AbstractStateMapper::PartialAssignmentIterator& a,
    const utils::default_sentinel_t&)
{
    return !a.done;
}

AbstractStateMapper::AbstractStateIterator::AbstractStateIterator(
    AbstractState offset,
    const std::vector<int>& indices,
    const std::vector<VariableInfo>& var_infos)
    : values_(indices.size(), 0)
    , domains_(indices.size())
    , multipliers_(indices.size())
    , state_(offset)
    , done(false)
{
    for (size_t i = 0; i != indices.size(); ++i) {
        const VariableInfo& info = var_infos[indices[i]];
        multipliers_[i] = info.multiplier;
        domains_[i] = info.domain;
    }
}

AbstractStateMapper::AbstractStateIterator&
AbstractStateMapper::AbstractStateIterator::operator++()
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

const AbstractState& AbstractStateMapper::AbstractStateIterator::operator*()
{
    return state_;
}

const AbstractState* AbstractStateMapper::AbstractStateIterator::operator->()
{
    return &state_;
}

bool operator==(
    const AbstractStateMapper::AbstractStateIterator& a,
    const utils::default_sentinel_t&)
{
    return a.done;
}

bool operator!=(
    const AbstractStateMapper::AbstractStateIterator& a,
    const utils::default_sentinel_t&)
{
    return !a.done;
}

AbstractStateMapper::AbstractStateMapper(
    Pattern pattern,
    const std::vector<int>& domains)
    : pattern_(std::move(pattern))
    , var_infos_(pattern_.size())
{
    assert(!pattern_.empty());
    assert(std::is_sorted(pattern_.begin(), pattern_.end()));

    constexpr int maxint = std::numeric_limits<int>::max();

    {
        VariableInfo& first_info = var_infos_[0];
        first_info.var = pattern_[0];
        first_info.multiplier = 1;
        first_info.partial_multiplier = 1;
    }

    for (unsigned i = 1; i < var_infos_.size(); ++i) {
        VariableInfo& prev_info = var_infos_[i - 1];
        VariableInfo& cur_info = var_infos_[i];

        assert(prev_info.var < static_cast<int>(domains.size()));
        const int d = domains[prev_info.var];
        prev_info.domain = d;

        if (prev_info.partial_multiplier > maxint / (d + 1)) {
            throw PatternTooLargeException();
        }

        cur_info.var = pattern_[i];
        cur_info.multiplier = prev_info.multiplier * d;
        cur_info.partial_multiplier = prev_info.partial_multiplier * (d + 1);
    }

    VariableInfo& last_info = var_infos_.back();
    assert(last_info.var < static_cast<int>(domains.size()));

    const int d = domains[last_info.var];
    last_info.domain = d;

    if (last_info.partial_multiplier > maxint / (d + 1)) {
        throw PatternTooLargeException();
    }

    num_states_ = last_info.multiplier * d;
    num_partial_states_ = last_info.partial_multiplier * (d + 1);
}

unsigned AbstractStateMapper::num_states() const
{
    return num_states_;
}

unsigned AbstractStateMapper::num_vars() const
{
    return var_infos_.size();
}

const Pattern& AbstractStateMapper::get_pattern() const
{
    return pattern_;
}

AbstractState
AbstractStateMapper::from_values(const std::vector<int>& values) const
{
    assert(values.size() == var_infos_.size());
    AbstractState res(0);
    auto it = values.begin();
    for (const VariableInfo& info : var_infos_) {
        res.id += info.multiplier * (*it++);
    }
    return res;
}

AbstractState AbstractStateMapper::from_values_partial(
    const std::vector<int>& indices,
    const std::vector<int>& values) const
{
    assert(values.size() == var_infos_.size());
    AbstractState res(0);
    for (int j : indices) {
        res.id += var_infos_[j].multiplier * values[j];
    }
    return res;
}

AbstractState AbstractStateMapper::from_values_partial(
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    AbstractState res(0);
    for (const auto& [idx, val] : sparse_values) {
        assert(utils::in_bounds(idx, var_infos_));
        assert(0 <= val && val < var_infos_[idx].domain);
        res.id += var_infos_[idx].multiplier * val;
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

        res.id += var_infos_[idx].multiplier * it->second;
    }

    return res;
}

AbstractState AbstractStateMapper::from_value_partial(int idx, int val) const
{
    return AbstractState(var_infos_[idx].multiplier * val);
}

int AbstractStateMapper::get_unique_partial_state_id(
    const std::vector<int>& indices,
    const std::vector<int>& values) const
{
    assert(values.size() == var_infos_.size());
    int id = 0;
    for (int j : indices) {
        id += var_infos_[j].partial_multiplier * (values[j] + 1);
    }
    return id;
}

int AbstractStateMapper::get_unique_partial_state_id(
    const std::vector<std::pair<int, int>>& pstate) const
{
    int id = 0;
    for (const auto& [var, val] : pstate) {
        id += var_infos_[var].partial_multiplier * (val + 1);
    }
    return id;
}

std::vector<int>
AbstractStateMapper::to_values(AbstractState abstract_state) const
{
    std::vector<int> values(var_infos_.size());
    for (size_t i = 0; i != var_infos_.size(); ++i) {
        const VariableInfo& info = var_infos_[i];
        values[i] = (abstract_state.id / info.multiplier) % info.domain;
    }
    return values;
}

void AbstractStateMapper::to_values(
    AbstractState abstract_state,
    std::vector<int>& values) const
{
    values.resize(var_infos_.size());
    for (size_t i = 0; i != var_infos_.size(); ++i) {
        const VariableInfo& info = var_infos_[i];
        values[i] = (abstract_state.id / info.multiplier) % info.domain;
    }
}

AbstractState AbstractStateMapper::convert(
    AbstractState abstract_state,
    const Pattern& pattern) const
{
    assert(std::includes(
        pattern_.begin(),
        pattern_.end(),
        pattern.begin(),
        pattern.end()));

    assert(!pattern.empty());

    AbstractState converted_state(0);
    int multiplier = 1;

    auto pattern_it = pattern.begin();
    auto pattern_end = pattern.end();

    const VariableInfo& first_info = var_infos_[0];
    if (first_info.var == *pattern_it) {
        converted_state.id += abstract_state.id % first_info.domain;
        multiplier *= first_info.domain;
        ++pattern_it;
    }

    for (int i = 1; pattern_it != pattern_end; ++pattern_it, ++i) {
        while (var_infos_[i].var != *pattern_it) {
            ++i;
        }

        const VariableInfo& cur_info = var_infos_[i];

        int value = (abstract_state.id / cur_info.multiplier) % cur_info.domain;
        converted_state.id += multiplier * value;
        multiplier *= cur_info.domain;
    }

    return converted_state;
}

AbstractStateMapper::PartialAssignmentIterator
AbstractStateMapper::cartesian_subsets_begin(
    std::vector<std::pair<int, int>> partial_state) const
{
    return PartialAssignmentIterator(std::move(partial_state), var_infos_);
}

utils::default_sentinel_t AbstractStateMapper::cartesian_subsets_end() const
{
    return utils::default_sentinel_t();
}

utils::RangeProxy<
    AbstractStateMapper::PartialAssignmentIterator,
    utils::default_sentinel_t>
AbstractStateMapper::cartesian_subsets(
    std::vector<std::pair<int, int>> partial_state) const
{
    return utils::
        RangeProxy<PartialAssignmentIterator, utils::default_sentinel_t>(
            cartesian_subsets_begin(std::move(partial_state)),
            cartesian_subsets_end());
}

AbstractStateMapper::AbstractStateIterator
AbstractStateMapper::partial_states_begin(
    AbstractState offset,
    std::vector<int> indices) const
{
    return AbstractStateIterator(offset, indices, var_infos_);
}

utils::default_sentinel_t AbstractStateMapper::partial_states_end() const
{
    return utils::default_sentinel_t();
}

utils::RangeProxy<
    AbstractStateMapper::AbstractStateIterator,
    utils::default_sentinel_t>
AbstractStateMapper::partial_states(
    AbstractState offset,
    std::vector<int> indices) const
{
    return utils::RangeProxy<AbstractStateIterator, utils::default_sentinel_t>(
        partial_states_begin(offset, indices),
        partial_states_end());
}

int AbstractStateMapper::get_multiplier(int var) const
{
    return get_multiplier_raw(get_index(var));
}

int AbstractStateMapper::get_multiplier_raw(int idx) const
{
    return var_infos_[idx].multiplier;
}

int AbstractStateMapper::get_domain_size(int var) const
{
    return get_domain_size_raw(get_index(var));
}

int AbstractStateMapper::get_domain_size_raw(int idx) const
{
    return var_infos_[idx].domain;
}

int AbstractStateMapper::get_index(int var) const
{
    auto it = std::find(pattern_.begin(), pattern_.end(), var);
    return it != pattern_.end() ? std::distance(pattern_.begin(), it) : -1;
}

AbstractStateToString::AbstractStateToString(
    std::shared_ptr<AbstractStateMapper> state_mapper)
    : state_mapper_(std::move(state_mapper))
{
}

std::string
AbstractStateToString::operator()(const StateID&, AbstractState state) const
{
    std::ostringstream out;
    std::vector<int> values = state_mapper_->to_values(state);
    for (unsigned i = 0; i < values.size(); i++) {
        const int var = state_mapper_->get_pattern()[i];
        out << (i > 0 ? ", " : "") << ::g_fact_names[var][values[i]];
    }
    return out.str();
}

} // namespace pdbs
} // namespace probabilistic
