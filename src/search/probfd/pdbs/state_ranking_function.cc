#include "probfd/pdbs/state_ranking_function.h"

#include "downward/abstract_task.h"

#include <cassert>
#include <limits>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace probfd::pdbs {

static constexpr auto maxint = std::numeric_limits<long long int>::max();

StateRankingFunction::StateRankingFunction(
    const VariablesProxy& variables,
    Pattern pattern)
    : pattern_(std::move(pattern))
    , var_infos_(pattern_.size())
{
    assert(std::is_sorted(pattern_.begin(), pattern_.end()));

    long long int multiplier = 1;

    for (auto [cur_info, var] : std::views::zip(var_infos_, pattern_)) {
        const int domain_size = variables[var].get_domain_size();
        cur_info.domain = domain_size;
        cur_info.multiplier = multiplier;

        if (multiplier > maxint / domain_size) {
            throw std::range_error("Construction of PDB would exceed "
                                   "std::numeric_limits<long long int>::max()");
        }

        multiplier *= domain_size;
    }

    num_states_ = multiplier;
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

StateRank StateRankingFunction::get_abstract_rank(const State& state) const
{
    StateRank res = 0;
    for (size_t i = 0; i != pattern_.size(); ++i) {
        res += rank_fact(i, state[pattern_[i]].get_value());
    }
    return res;
}

int StateRankingFunction::rank_fact(int idx, int val) const
{
    return var_infos_[idx].multiplier * val;
}

std::vector<int> StateRankingFunction::unrank(StateRank state_rank) const
{
    std::vector<int> values(var_infos_.size());
    for (size_t i = 0; i != var_infos_.size(); ++i) {
        values[i] = value_of(state_rank, i);
    }
    return values;
}

int StateRankingFunction::value_of(StateRank state_rank, int idx) const
{
    const VariableInfo& info = var_infos_[idx];
    return (state_rank / info.multiplier) % info.domain;
}

bool StateRankingFunction::next_partial_assignment(
    std::vector<FactPair>& partial_state) const
{
    for (int i = partial_state.size() - 1; i >= 0; --i) {
        auto& [var, val] = partial_state[i];
        const int next = val + 1;

        if (next < var_infos_[var].domain) {
            val = next;
            return true;
        }

        val = 0;
    }

    return false;
}

bool StateRankingFunction::next_rank(
    StateRank& abstract_state_rank,
    std::span<int> mutable_variables) const
{
    for (int var : mutable_variables) {
        const int domain = var_infos_[var].domain;
        const int multiplier = var_infos_[var].multiplier;
        const int value = (abstract_state_rank / multiplier) % domain;

        if (value + 1 < domain) {
            abstract_state_rank += multiplier;
            return true;
        }

        abstract_state_rank -= value * multiplier;
    }

    return false;
}

long long int StateRankingFunction::get_multiplier(int var) const
{
    return var_infos_[var].multiplier;
}

int StateRankingFunction::get_domain_size(int var) const
{
    return var_infos_[var].domain;
}

StateRankToString::StateRankToString(
    VariablesProxy variables,
    const StateRankingFunction& state_mapper)
    : variables_(variables)
    , state_mapper_(state_mapper)
{
}

std::string StateRankToString::operator()(StateRank state) const
{
    using namespace std::views;

    std::ostringstream out;

    const Pattern& pattern = state_mapper_.get_pattern();
    std::vector<int> values = state_mapper_.unrank(state);

    if (pattern.empty()) return "";

    out << variables_[pattern.front()].get_fact(values.front()).get_name();

    for (const auto [var, val] : zip(pattern, values) | drop(1)) {
        out << ", " << variables_[var].get_fact(val).get_name();
    }

    return out.str();
}

} // namespace probfd::pdbs
