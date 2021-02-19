#include "probabilistic_projection.h"

#include "../../../global_operator.h"
#include "../../../successor_generator.h"
#include "../../analysis_objective.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../globals.h"

#include <algorithm>
#include <deque>
#include <set>
#include <numeric>

namespace probabilistic {
namespace pdbs {

ProbabilisticProjection::ProbabilisticProjection(
    const std::vector<int>& variables,
    const std::vector<int>& domains)
    : var_index_(domains.size(), -1)
    , state_mapper_(std::make_shared<AbstractStateMapper>(variables, domains))
    , initial_state_((*state_mapper_)(::g_initial_state_data))
    , goal_states_()
    , progression_aops_generator_(nullptr)
    , regression_aops_generator_(nullptr)
{
    for (size_t i = 0; i < variables.size(); ++i) {
        var_index_[variables[i]] = i;
    }

    setup_abstract_goal();
}

std::shared_ptr<AbstractStateMapper>
ProbabilisticProjection::get_abstract_state_mapper() const
{
    return state_mapper_;
}

QualitativeResultStore&
ProbabilisticProjection::get_abstract_goal_states()
{
    return goal_states_;
}

void
ProbabilisticProjection::setup_abstract_goal()
{
    const std::vector<int>& variables = state_mapper_->get_variables();
    const std::vector<int>& domains = state_mapper_->get_domains();
    unsigned num_goal_states = 1;

    std::vector<int> goal(variables.size(), 0);
    for (const auto& var_val : g_goal) {
        const int idx = var_index_[var_val.first];
        if (idx != -1) {
            std::pair<int, int> p(idx, var_val.second);
            // Insert sorted
            auto it = std::upper_bound(
                projected_goal_.begin(), projected_goal_.end(), p);
            projected_goal_.insert(it, p);
            goal[idx] = var_val.second;
        }
    }

    std::vector<int> non_goal_vars;
    int v = 0;
    for (const auto& var_val : projected_goal_) {
        while (v < var_val.first) {
            num_goal_states = num_goal_states * domains[v];
            non_goal_vars.push_back(v++);
        }
        v++;
    }
    while (v < static_cast<int>(variables.size())) {
        num_goal_states = num_goal_states * domains[v];
        non_goal_vars.push_back(v++);
    }

    if (projected_goal_.empty()) {
        goal_states_.negate_all();
    } else if (2 * num_goal_states >= state_mapper_->size()) {
        // Goal state majority -> insert non-goal states, negate the lazy set
        non_goal_vars.clear();

        std::vector<int> free_vars(variables.size());
        std::iota(free_vars.begin(), free_vars.end(), 0);

        std::fill(goal.begin(), goal.end(), 0);
        AbstractState part_goal(0);

        for (const auto& [g_var, g_val] : projected_goal_) {
            // Fix the goal variable...
            free_vars.erase(
                std::lower_bound(
                    free_vars.begin(), free_vars.end(), g_var));

            // ...to any non-goal value to obtain the non-goal states
            for (int val = 0; val < domains[g_var]; ++val) {
                if (val != g_val) {
                    goal[g_var] = val;
                    part_goal += state_mapper_->from_value_partial(g_var, val);
                    state_mapper_->enumerate(
                        free_vars,
                        goal,
                        [this, part_goal](
                            AbstractState state, const std::vector<int>&) {
                            goal_states_.set(part_goal + state, true);
                        });
                    part_goal -= state_mapper_->from_value_partial(g_var, val);
                }
            }

            goal[g_var] = g_val;
            part_goal += state_mapper_->from_value_partial(g_var, g_val);
        }

        // Negate lazy set
        goal_states_.negate_all();
    } else {
        // Goal state minority -> insert goal states
        AbstractState base_goal = state_mapper_->from_values(goal);

        state_mapper_->enumerate(
            non_goal_vars,
            goal,
            [this, base_goal](AbstractState missing, const std::vector<int>&) {
                goal_states_.set(base_goal + missing, true);
            });
    }
}

struct OutcomeInfo {
    explicit OutcomeInfo(value_type::value_t prob)
        : base_effect(0)
        , probability(prob)
    {
    }
    OutcomeInfo(OutcomeInfo&&) = default;
    AbstractState base_effect;
    value_type::value_t probability;
    std::vector<int> missing_pres;
};

void
ProbabilisticProjection::setup_abstract_operators()
{
    if (progression_aops_generator_ != nullptr) {
        return;
    }

    const std::vector<int>& variables = state_mapper_->get_variables();

    std::vector<std::vector<std::pair<int, int>>> preconditions;
    abstract_operators_.reserve(g_operators.size());
    preconditions.reserve(g_operators.size());

    using footprint_t =
        std::vector<std::pair<AbstractState, value_type::value_t>>;

    std::set<footprint_t> operators;
    footprint_t footprint;

    for (unsigned op_id = 0; op_id < g_operators.size(); ++op_id) {
        const ProbabilisticOperator& op = g_operators[op_id];

        std::vector<int> pre_var_indices;
        std::vector<int> eff_no_pre_var_indices;

        std::vector<int> pre_assignment(variables.size(), -1);
        {
            for (const auto& [var, val] : op.get_preconditions()) {
                const int idx = var_index_[var];
                if (idx != -1) {
                    pre_assignment[idx] = val;
                    pre_var_indices.push_back(idx);
                }
            }
        }

        std::vector<OutcomeInfo> outcomes;
        value_type::value_t self_loop_prob = value_type::zero;
        for (const ProbabilisticOutcome& out : op) {
            const std::vector<GlobalEffect>& effects = out.op->get_effects();
            const value_type::value_t prob = out.prob;

            bool self_loop = true;
            OutcomeInfo info(prob);
            {
                for (const auto& [eff_var, eff_val, _] : effects) {
                    const int idx = var_index_[eff_var];

                    if (idx != -1) {
                        self_loop = false;
                        info.base_effect += state_mapper_->from_value_partial(
                            idx, eff_val);
                        if (pre_assignment[idx] == -1) {
                            eff_no_pre_var_indices.push_back(idx);
                            info.missing_pres.push_back(idx);
                        } else {
                            info.base_effect -=
                                state_mapper_->from_value_partial(
                                    idx, pre_assignment[idx]);
                        }
                    }
                }
            }

            if (self_loop) {
                self_loop_prob += prob;
            } else {
                outcomes.emplace_back(std::move(info));
            }
        }

        if (!outcomes.empty()) {
            auto setify = [](auto& v) {
                std::sort(v.begin(), v.end());
                v.erase(std::unique(v.begin(), v.end()), v.end());
            };
            setify(eff_no_pre_var_indices);

            pre_var_indices.insert(
                pre_var_indices.end(),
                eff_no_pre_var_indices.begin(),
                eff_no_pre_var_indices.end());
            std::sort(pre_var_indices.begin(), pre_var_indices.end());

            if (value_type::approx_greater()(
                    self_loop_prob, value_type::zero)) {
                outcomes.emplace_back(self_loop_prob);
            }

            auto add_operator = [this,
                                 op_id,
                                 &operators,
                                 &footprint,
                                 &preconditions,
                                 &outcomes,
                                 &pre_var_indices](
                                    const AbstractState&,
                                    const std::vector<int>& values)
            {
                AbstractOperator new_op(op_id);
                new_op.pre =
                    state_mapper_->from_values_partial(pre_var_indices, values);
#ifndef NDEBUG
                value_type::value_t sum = value_type::zero;
#endif
                for (const auto& out : outcomes) {
                    const auto& [base_effect, probability, missing_pres] = out;
#ifndef NDEBUG
                    assert(value_type::approx_greater()(
                        probability, value_type::zero));
                    sum += probability;
#endif
                    new_op.outcomes.add(
                        base_effect
                            - state_mapper_->from_values_partial(
                                missing_pres, values),
                        probability);
                }

                assert(value_type::approx_equal()(sum, value_type::one));

                for (const auto& out : new_op.outcomes) {
                    footprint.emplace_back(out.first, out.second);
                }

                std::sort(footprint.begin(), footprint.end());
                footprint.emplace_back(-1, 0);
                for (int pre_var_index : pre_var_indices) {
                    footprint.emplace_back(pre_var_index, 0);
                    footprint.emplace_back(values[pre_var_index], 0);
                }
                if (operators.insert(footprint).second) {
                    abstract_operators_.emplace_back(std::move(new_op));
                    preconditions.emplace_back(pre_var_indices.size());
                    auto& precons = preconditions.back();
                    for (size_t j = 0; j < pre_var_indices.size(); ++j) {
                        const int idx = pre_var_indices[j];
                        precons[j] = std::make_pair(idx, values[idx]);
                    }
                }
                footprint.clear();
            };
            state_mapper_->enumerate(
                eff_no_pre_var_indices, pre_assignment, add_operator);
        }
    }

    std::vector<const AbstractOperator*> opptrs(abstract_operators_.size());
    for (size_t i = 0; i < abstract_operators_.size(); ++i) {
        opptrs[i] = &abstract_operators_[i];
    }

    assert(abstract_operators_.size() == preconditions.size());

    progression_aops_generator_ = std::make_shared<
        successor_generator::SuccessorGenerator<const AbstractOperator*>>(
        state_mapper_->get_domains(), preconditions, opptrs);
}

void
ProbabilisticProjection::prepare_regression()
{
    if (regression_aops_generator_ != nullptr) {
        return;
    }

    const std::vector<int>& pattern = state_mapper_->get_variables();

    std::vector<std::vector<std::pair<int, int>>> progressions;
    progressions.reserve(::g_operators.size());
    std::vector<AbstractState> operators;

    {
        utils::HashSet<std::pair<AbstractState, AbstractState>> operator_set;

        for (const auto& op : ::g_operators) {
            std::vector<std::pair<int, int>> projected_eff;
            std::vector<int> eff_no_pre;
            std::vector<int> precondition(pattern.size(), -1);
            {
                for (const auto& [pre_var, pre_val] : op.get_preconditions()) {
                    const int idx = var_index_[pre_var];
                    if (idx != -1) {
                        precondition[idx] = pre_val;
                    }
                }
            }

            AbstractState pre(0);
            AbstractState eff(0);
            {
                for (const auto& [eff_var, eff_val, _] : op.get_effects()) {
                    const int idx = var_index_[eff_var];
                    if (idx != -1) {
                        projected_eff.emplace_back(idx, eff_val);
                        eff += state_mapper_->from_value_partial(idx, eff_val);
                        if (precondition[idx] == -1) {
                            eff_no_pre.push_back(idx);
                        } else {
                            pre += state_mapper_->from_value_partial(
                                idx, precondition[idx]);
                        }
                    }
                }
            }

            if (projected_eff.empty()) {
                continue;
            }

            std::sort(projected_eff.begin(), projected_eff.end());
            std::sort(eff_no_pre.begin(), eff_no_pre.end());
            state_mapper_->enumerate(
                eff_no_pre,
                precondition,
                [&progressions,
                 &operators,
                 &operator_set,
                 &projected_eff,
                 &pre,
                 &eff](AbstractState missing, const std::vector<int>&) {
                    const AbstractState regression = pre + missing - eff;
                    if (regression == AbstractState(0)) {
                        return;
                    }
                    std::pair<AbstractState, AbstractState> footprint(
                        eff, regression);
                    if (operator_set.insert(footprint).second) {
                        progressions.push_back(projected_eff);
                        operators.push_back(regression);
                    }
                });
        }
    }

    regression_aops_generator_ = std::make_shared<
        successor_generator::SuccessorGenerator<AbstractState>>(
        state_mapper_->get_domains(), progressions, operators);
}

} // namespace pdbs
} // namespace probabilistic
