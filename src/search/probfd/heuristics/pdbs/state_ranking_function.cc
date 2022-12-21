#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "utils/collections.h"
#include "utils/exceptions.h"

#include "legacy/global_state.h"
#include "legacy/globals.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <sstream>

namespace probfd {
namespace heuristics {
namespace pdbs {

StateRankingFunction::PartialAssignmentIterator::PartialAssignmentIterator(
    std::vector<std::pair<int, int>> partial_state,
    const std::vector<VariableInfo>& var_infos)
    : partial_state_(std::move(partial_state))
    , var_infos_(var_infos)
    , done(false)
{
}

StateRankingFunction::PartialAssignmentIterator&
StateRankingFunction::PartialAssignmentIterator::operator++()
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

StateRankingFunction::PartialAssignmentIterator&
StateRankingFunction::PartialAssignmentIterator::operator--()
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

StateRankingFunction::PartialAssignmentIterator::reference
StateRankingFunction::PartialAssignmentIterator::operator*()
{
    return partial_state_;
}

StateRankingFunction::PartialAssignmentIterator::pointer
StateRankingFunction::PartialAssignmentIterator::operator->()
{
    return &partial_state_;
}

bool operator==(
    const StateRankingFunction::PartialAssignmentIterator& a,
    const utils::default_sentinel_t&)
{
    return a.done;
}

bool operator!=(
    const StateRankingFunction::PartialAssignmentIterator& a,
    const utils::default_sentinel_t&)
{
    return !a.done;
}

StateRankingFunction::StateRankIterator::StateRankIterator(
    StateRank offset,
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

StateRankingFunction::StateRankIterator&
StateRankingFunction::StateRankIterator::operator++()
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

const StateRank& StateRankingFunction::StateRankIterator::operator*()
{
    return state_;
}

const StateRank* StateRankingFunction::StateRankIterator::operator->()
{
    return &state_;
}

bool operator==(
    const StateRankingFunction::StateRankIterator& a,
    const utils::default_sentinel_t&)
{
    return a.done;
}

bool operator!=(
    const StateRankingFunction::StateRankIterator& a,
    const utils::default_sentinel_t&)
{
    return !a.done;
}

StateRankingFunction::StateRankingFunction(
    Pattern pattern,
    const std::vector<int>& domains)
    : pattern_(std::move(pattern))
    , var_infos_(pattern_.size())
{
    assert(!pattern_.empty());
    assert(std::is_sorted(pattern_.begin(), pattern_.end()));

    constexpr long long int maxint = std::numeric_limits<long long int>::max();

    {
        VariableInfo& first_info = var_infos_[0];
        first_info.multiplier = 1;
        first_info.partial_multiplier = 1;
    }

    for (unsigned i = 1; i < var_infos_.size(); ++i) {
        VariableInfo& prev_info = var_infos_[i - 1];
        VariableInfo& cur_info = var_infos_[i];

        const int d = domains[pattern_[i - 1]];
        prev_info.domain = d;

        if (prev_info.partial_multiplier > maxint / (d + 1)) {
            throw std::range_error("Construction of PDB would exceed "
                                   "std::numeric_limits<long long int>::max()");
        }

        cur_info.multiplier = prev_info.multiplier * d;
        cur_info.partial_multiplier = prev_info.partial_multiplier * (d + 1);
    }

    VariableInfo& last_info = var_infos_.back();

    const int d = domains[pattern_.back()];
    last_info.domain = d;

    if (last_info.multiplier > maxint / d) {
        throw std::range_error("Construction of PDB would exceed "
                               "std::numeric_limits<long long int>::max()");
    }

    num_states_ = last_info.multiplier * d;
}

unsigned StateRankingFunction::num_states() const
{
    return num_states_;
}

unsigned StateRankingFunction::num_vars() const
{
    return var_infos_.size();
}

const Pattern& StateRankingFunction::get_pattern() const
{
    return pattern_;
}

StateRank StateRankingFunction::from_values_partial(
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    StateRank res(0);
    for (const auto& [idx, val] : sparse_values) {
        assert(utils::in_bounds(idx, var_infos_));
        assert(0 <= val && val < var_infos_[idx].domain);
        res.id += var_infos_[idx].multiplier * val;
    }
    return res;
}

StateRank StateRankingFunction::from_values_partial(
    const std::vector<int>& indices,
    const std::vector<std::pair<int, int>>& sparse_values) const
{
    StateRank res(0);

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

StateRank StateRankingFunction::from_fact(int idx, int val) const
{
    return StateRank(var_infos_[idx].multiplier * val);
}

long long int StateRankingFunction::get_unique_partial_state_id(
    const std::vector<std::pair<int, int>>& pstate) const
{
    long long int id = 0;
    for (const auto& [var, val] : pstate) {
        id += var_infos_[var].partial_multiplier * (val + 1);
    }
    return id;
}

std::vector<int> StateRankingFunction::unrank(StateRank state_rank) const
{
    std::vector<int> values(var_infos_.size());
    for (size_t i = 0; i != var_infos_.size(); ++i) {
        const VariableInfo& info = var_infos_[i];
        values[i] = (state_rank.id / info.multiplier) % info.domain;
    }
    return values;
}

StateRank
StateRankingFunction::convert(StateRank state_rank, const Pattern& pattern)
    const
{
    assert(std::includes(
        pattern_.begin(),
        pattern_.end(),
        pattern.begin(),
        pattern.end()));

    assert(!pattern.empty());

    StateRank converted_state(0);
    long long int multiplier = 1;

    auto pattern_it = pattern.begin();
    auto pattern_end = pattern.end();

    const VariableInfo& first_info = var_infos_[0];
    if (pattern_[0] == *pattern_it) {
        converted_state.id += state_rank.id % first_info.domain;
        multiplier *= first_info.domain;
        ++pattern_it;
    }

    for (int i = 1; pattern_it != pattern_end; ++pattern_it, ++i) {
        while (pattern_[i] != *pattern_it) {
            ++i;
        }

        const VariableInfo& cur_info = var_infos_[i];

        int value = (state_rank.id / cur_info.multiplier) % cur_info.domain;
        converted_state.id += multiplier * value;
        multiplier *= cur_info.domain;
    }

    return converted_state;
}

StateRankingFunction::PartialAssignmentIterator
StateRankingFunction::partial_assignments_begin(
    std::vector<std::pair<int, int>> partial_state) const
{
    return PartialAssignmentIterator(std::move(partial_state), var_infos_);
}

utils::default_sentinel_t StateRankingFunction::partial_assignments_end() const
{
    return utils::default_sentinel_t();
}

utils::RangeProxy<
    StateRankingFunction::PartialAssignmentIterator,
    utils::default_sentinel_t>
StateRankingFunction::partial_assignments(
    std::vector<std::pair<int, int>> partial_state) const
{
    return utils::
        RangeProxy<PartialAssignmentIterator, utils::default_sentinel_t>(
            partial_assignments_begin(std::move(partial_state)),
            partial_assignments_end());
}

StateRankingFunction::StateRankIterator StateRankingFunction::state_ranks_begin(
    StateRank offset,
    std::vector<int> indices) const
{
    return StateRankIterator(offset, indices, var_infos_);
}

utils::default_sentinel_t StateRankingFunction::state_ranks_end() const
{
    return utils::default_sentinel_t();
}

utils::RangeProxy<
    StateRankingFunction::StateRankIterator,
    utils::default_sentinel_t>
StateRankingFunction::state_ranks(StateRank offset, std::vector<int> indices)
    const
{
    return utils::RangeProxy<StateRankIterator, utils::default_sentinel_t>(
        state_ranks_begin(offset, indices),
        state_ranks_end());
}

long long int StateRankingFunction::get_multiplier(int var) const
{
    return var_infos_[var].multiplier;
}

int StateRankingFunction::get_domain_size(int var) const
{
    return var_infos_[var].domain;
}

int StateRankingFunction::get_index(int var) const
{
    auto it = std::find(pattern_.begin(), pattern_.end(), var);
    return it != pattern_.end() ? std::distance(pattern_.begin(), it) : -1;
}

StateRankToString::StateRankToString(
    std::shared_ptr<StateRankingFunction> state_mapper)
    : state_mapper_(std::move(state_mapper))
{
}

std::string StateRankToString::operator()(const StateID&, StateRank state) const
{
    std::ostringstream out;
    std::vector<int> values = state_mapper_->unrank(state);
    for (unsigned i = 0; i < values.size(); i++) {
        const int var = state_mapper_->get_pattern()[i];
        out << (i > 0 ? ", " : "") << legacy::g_fact_names[var][values[i]];
    }
    return out.str();
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
