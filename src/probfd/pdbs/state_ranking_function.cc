#include "probfd/pdbs/state_ranking_function.h"

#include <ranges>
#include <sstream>
#include <utility>

using namespace downward;

namespace probfd::pdbs {

StateRankingFunction::StateRankingFunction(
    const VariablesProxy& variables,
    Pattern pattern)
    : pattern_(std::move(pattern))
    , enumerator_(pattern_ | std::views::transform([&](int var) {
                      return variables[var].get_domain_size();
                  }))
{
}

unsigned StateRankingFunction::num_states() const
{
    return enumerator_.num_assignments();
}

unsigned StateRankingFunction::num_vars() const
{
    return enumerator_.num_vars();
}

const Pattern& StateRankingFunction::get_pattern() const
{
    return pattern_;
}

const AssignmentEnumerator& StateRankingFunction::get_enumerator() const
{
    return enumerator_;
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
    return enumerator_.rank_fact(idx, val);
}

std::vector<int> StateRankingFunction::unrank(StateRank state_rank) const
{
    return enumerator_.unrank(state_rank);
}

int StateRankingFunction::value_of(StateRank state_rank, int idx) const
{
    return enumerator_.value_of(state_rank, idx);
}

bool StateRankingFunction::next_rank(
    StateRank& abstract_state_rank,
    std::span<int> mutable_variables) const
{
    return enumerator_.next_index(abstract_state_rank, mutable_variables);
}

long long int StateRankingFunction::get_multiplier(int var) const
{
    return enumerator_.get_multiplier(var);
}

int StateRankingFunction::get_domain_size(int var) const
{
    return enumerator_.get_domain_size(var);
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
