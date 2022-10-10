#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_TYPES_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_TYPES_H

#include <vector>

#include "../../../../../global_operator.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

struct Flaw {
    bool is_goal_violation;
    int solution_index;
    int variable;

    Flaw(bool is_goal_violation, int solution_index, int variable)
        : is_goal_violation(is_goal_violation)
        , solution_index(solution_index)
        , variable(variable)
    {
    }
};

using FlawList = std::vector<Flaw>;

struct ExplicitGState {
    std::vector<int> values;

    ExplicitGState(std::vector<int> values)
        : values(std::move(values))
    {
    }

    size_t get_hash() const
    {
        std::size_t res = 0;
        for (size_t i = 0; i < values.size(); ++i) {
            res += g_variable_domain[i] * values[i];
        }
        return res;
    }

    int& operator[](int i) { return values[i]; }

    const int& operator[](int i) const { return values[i]; }

    ExplicitGState get_successor(const GlobalOperator& op) const
    {
        assert(!op.is_axiom());

        ExplicitGState s(*this);

        for (auto& eff : op.get_effects()) {
            assert(eff.conditions.empty());
            s[eff.var] = eff.val;
        }

        return s;
    }

    bool is_goal() const
    {
        for (auto& [goal_var, goal_val] : g_goal) {
            if (values[goal_var] != goal_val) {
                return false;
            }
        }

        return true;
    }

    friend bool operator==(const ExplicitGState& a, const ExplicitGState& b)
    {
        return a.values == b.values;
    }
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

namespace std {
template <>
struct hash<probfd::heuristics::pdbs::pattern_selection::ExplicitGState> {
    size_t operator()(
        const probfd::heuristics::pdbs::pattern_selection::ExplicitGState&
            state) const
    {
        return state.get_hash();
    }
};
} // namespace std

#endif