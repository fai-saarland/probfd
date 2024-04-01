#include "probfd/pdbs/projection_state_space.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/cost_function.h"
#include "probfd/distribution.h"
#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <compare>
#include <functional>
#include <span>
#include <tuple>

using namespace std::views;

namespace probfd::pdbs {

namespace {

struct Outcome {
    ProbabilisticEffectsProxy proxy;
    std::pair<
        ProxyIterator<ProbabilisticEffectsProxy>,
        ProxyIterator<ProbabilisticEffectsProxy>>
        effect_range;

    explicit Outcome(ProbabilisticOutcomeProxy outcome)
        : proxy(outcome.get_effects())
        , effect_range(proxy.begin(), proxy.end())
    {
    }
};

struct OperatorInfo {
    struct ProbabilisticOffset {
        StateRank rank_offset = 0;
        value_t probability;
    };

    struct MissingPreconditionInfo {
        int precondition_index;
        std::vector<std::vector<ProbabilisticOffset>::iterator>
            affected_offsets;
    };

    using offset_iterator = std::vector<ProbabilisticOffset>::iterator;

    std::vector<ProbabilisticOffset> effect_infos;
    std::vector<MissingPreconditionInfo> missing_info;
};

} // namespace

static void compute_projection_operator_info(
    ProbabilisticOperatorProxy op,
    const StateRankingFunction& ranking_function,
    std::vector<FactPair>& precondition,
    OperatorInfo& operator_info)
{
    std::vector<Outcome> outcomes;

    operator_info.effect_infos.reserve(op.get_outcomes().size());
    outcomes.reserve(op.get_outcomes().size());

    for (ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
        outcomes.emplace_back(outcome);
        operator_info.effect_infos.emplace_back(0, outcome.get_probability());
    }

    auto op_preconditions = op.get_preconditions();
    auto it = op_preconditions.begin();
    auto end = op_preconditions.end();

    const Pattern& pattern = ranking_function.get_pattern();

    for (size_t i = 0; i != pattern.size(); ++i) {
        int var = pattern[i];

        for (;;) {
            if (it == end) { // No precondition on this variable
            no_precondition:;
                bool has_effect = false;
                std::vector<OperatorInfo::offset_iterator> affected_offsets;

                auto out_it = outcomes.begin();
                auto out_end = outcomes.end();
                auto out_info_it = operator_info.effect_infos.begin();

                for (; out_it != out_end; ++out_it, ++out_info_it) {
                    auto& [eff_it, eff_end] = out_it->effect_range;
                    while (eff_it != eff_end) {
                        FactPair eff_fact = (*eff_it).get_fact().get_pair();

                        // Skip effect on var not in patterm
                        if (eff_fact.var < var) {
                            ++eff_it;
                            continue;
                        }

                        // Effect on this variable
                        if (eff_fact.var == var) {
                            has_effect = true;
                            affected_offsets.push_back(out_info_it);
                            out_info_it->rank_offset +=
                                ranking_function.rank_fact(i, eff_fact.value);
                            ++eff_it;
                        }

                        break;
                    }
                }

                if (has_effect) {
                    operator_info.missing_info.emplace_back(
                        precondition.size(),
                        affected_offsets);
                    precondition.emplace_back(i, 0);
                }

                break;
            }

            FactPair pre_fact = (*it).get_pair();

            // Skip precondition on var not in patterm
            if (pre_fact.var < var) {
                ++it;
                continue;
            }

            if (pre_fact.var > var) goto no_precondition;

            int pre_val = (*it).get_value();
            precondition.emplace_back(i, pre_val);

            auto out_it = outcomes.begin();
            auto out_end = outcomes.end();
            auto out_info_it = operator_info.effect_infos.begin();

            for (; out_it != out_end; ++out_it, ++out_info_it) {
                auto& [eff_it, eff_end] = out_it->effect_range;
                while (eff_it != eff_end) {
                    FactPair eff_fact = (*eff_it).get_fact().get_pair();

                    // Skip effect on var not in patterm
                    if (eff_fact.var < var) {
                        ++eff_it;
                        continue;
                    }

                    // Effect on this variable
                    if (eff_fact.var == var) {
                        out_info_it->rank_offset += ranking_function.rank_fact(
                            i,
                            eff_fact.value - pre_val);
                        ++eff_it;
                    }

                    break;
                }
            }

            ++it;

            break;
        }
    }
}

ProjectionStateSpace::ProjectionStateSpace(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const StateRankingFunction& ranking_function,
    bool operator_pruning,
    double max_time)
    : match_tree_(task_proxy.get_operators().size())
    , parent_cost_function_(&task_cost_function)
    , goal_state_flags_(ranking_function.num_states(), false)
{
    utils::CountdownTimer timer(max_time);

    const Pattern& pattern = ranking_function.get_pattern();

    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    // Generate the abstract operators for each probabilistic operator
    for (const ProbabilisticOperatorProxy& op : operators) {
        timer.throw_if_expired();

        const OperatorID operator_id(op.get_id());
        const value_t cost = task_cost_function.get_action_cost(operator_id);

        std::vector<FactPair> precondition;
        OperatorInfo operator_info;

        // Compute projection precondition, as well as all variables in
        // U_i dom(eff_i) \ dom(pre).
        // Also compute the offsets for each outcome, assuming missing
        // preconditions have value 0.
        compute_projection_operator_info(
            op,
            ranking_function,
            precondition,
            operator_info);

        // Advances through all partial assignments p with
        // dom(p) = dom(pre) U (U_i dom(eff_i) \ \dom(pre))
        // and updates the rank offset for each outcome accordingly
        auto next_precondition = [&](auto& missing, auto& precondition) {
            for (const auto& [missing_idx, affected_offsets] : missing) {
                auto& [var, val] = precondition[missing_idx];
                const int next = val + 1;
                const int domain = ranking_function.get_domain_size(var);
                const int multiplier = ranking_function.get_multiplier(var);

                if (next < ranking_function.get_domain_size(var)) {
                    val = next;
                    for (const auto& it : affected_offsets) {
                        it->rank_offset -= multiplier;
                    }
                    return true;
                }

                val = 0;
                for (const auto& it : affected_offsets) {
                    it->rank_offset += (domain - 1) * multiplier;
                }
            }

            return false;
        };

        do {
            timer.throw_if_expired();

            // Generate the progression operator
            ProjectionOperator new_op(
                operator_id,
                cost,
                operator_info.effect_infos);

            // Now add the progression operators to the match tree
            match_tree_.insert(
                ranking_function,
                std::move(new_op),
                precondition,
                operator_pruning);
        } while (next_precondition(operator_info.missing_info, precondition));
    }

    const GoalsProxy task_goals = task_proxy.get_goals();

    std::vector<int> non_goal_vars;
    StateRank base(0);

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    const int num_variables = static_cast<int>(pattern.size());

    auto goal_it = task_goals.begin();
    const auto goal_end = task_goals.end();

    for (int v = 0; v != num_variables;) {
        const int p_var = pattern[v];
        const FactProxy goal_fact = *goal_it;
        const int g_var = goal_fact.get_variable().get_id();

        if (p_var < g_var) {
            non_goal_vars.push_back(v++);
        } else {
            if (p_var == g_var) {
                base += ranking_function.rank_fact(v++, goal_fact.get_value());
            }

            if (++goal_it == goal_end) {
                while (v < num_variables) {
                    non_goal_vars.push_back(v++);
                }
                break;
            }
        }
    }

    assert(non_goal_vars.size() != pattern.size()); // No goal no fun.

    do {
        goal_state_flags_[base] = true;
    } while (ranking_function.next_rank(base, non_goal_vars));
}

StateID ProjectionStateSpace::get_state_id(StateRank state)
{
    return state;
}

StateRank ProjectionStateSpace::get_state(StateID id)
{
    return StateRank(id);
}

void ProjectionStateSpace::generate_applicable_actions(
    StateRank state,
    std::vector<const ProjectionOperator*>& aops)
{
    match_tree_.get_applicable_operators(state, aops);
}

void ProjectionStateSpace::generate_action_transitions(
    StateRank state,
    const ProjectionOperator* op,
    Distribution<StateID>& result)
{
    for (const auto& [offset, probability] : op->outcome_offsets_) {
        result.add_probability(state + offset, probability);
    }
}

void ProjectionStateSpace::generate_all_transitions(
    StateRank state,
    std::vector<const ProjectionOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    generate_applicable_actions(state, aops);
    result.reserve(aops.size());
    for (const ProjectionOperator* op : aops) {
        generate_action_transitions(state, op, result.emplace_back());
    }
}

void ProjectionStateSpace::generate_all_transitions(
    StateRank state,
    std::vector<TransitionType>& transitions)
{
    match_tree_.generate_all_transitions(state, transitions, *this);
}

bool ProjectionStateSpace::is_goal(StateRank state) const
{
    return goal_state_flags_[state];
}

value_t ProjectionStateSpace::get_non_goal_termination_cost() const
{
    return parent_cost_function_->get_non_goal_termination_cost();
}

value_t ProjectionStateSpace::get_action_cost(const ProjectionOperator* op)
{
    return op->cost;
}

} // namespace probfd::pdbs
