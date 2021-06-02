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

// TODO move to utils
template <typename T, typename A>
void setify(std::vector<T, A>& v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
}

struct NoGoalVariableException : std::exception {
    const char* what() const noexcept override {
        return "Construction of a PDB without a goal "
            "variable is intentionally disallowed!";
    }
};

ProbabilisticProjection::
ProbabilisticProjection(const Pattern& pattern, const std::vector<int>& domains)
    : var_index_(domains.size(), -1)
    , state_mapper_(new AbstractStateMapper(pattern, domains))
    , initial_state_((*state_mapper_)(::g_initial_state_data))
{
    for (size_t i = 0; i < pattern.size(); ++i) {
        var_index_[pattern[i]] = i;
    }

    setup_abstract_goal();
    prepare_progression();
}

std::shared_ptr<AbstractStateMapper>
ProbabilisticProjection::get_abstract_state_mapper() const
{
    return state_mapper_;
}

std::unordered_set<AbstractState>&
ProbabilisticProjection::get_abstract_goal_states()
{
    return goal_states_;
}

unsigned int ProbabilisticProjection::num_states() const {
    return state_mapper_->size();
}

const Pattern& ProbabilisticProjection::get_pattern() const {
    return state_mapper_->get_pattern();
}

void ProbabilisticProjection::setup_abstract_goal()
{
    const Pattern& variables = state_mapper_->get_pattern();
    std::vector<int> non_goal_vars;

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    int v = 0;
    for (const auto [var, val] : g_goal) {
        const int idx = var_index_[var];
        if (idx == -1) continue;

        while (v < idx) {
            non_goal_vars.push_back(v++);
        }

        sparse_goal_.emplace_back(v, val);
        v++;
    }
    while (v < static_cast<int>(variables.size())) {
        non_goal_vars.push_back(v++);
    }

    assert(non_goal_vars.size() + sparse_goal_.size() == variables.size());

    // No goal no fun. Don't do it.
    if (sparse_goal_.empty()) {
        throw NoGoalVariableException();
    }
    
    AbstractState base_goal = state_mapper_->from_values_partial(sparse_goal_);
    state_mapper_->for_each_partial_state(
        base_goal, non_goal_vars, &ProbabilisticProjection::add_to_goals, this);
}

struct OutcomeInfo {
    explicit OutcomeInfo(value_type::value_t prob)
        : base_effect(0), probability(prob) { }
    AbstractState base_effect;
    value_type::value_t probability;
    std::vector<int> missing_pres;
};

void
ProbabilisticProjection::prepare_progression()
{
    const Pattern& variables = state_mapper_->get_pattern();

    std::vector<std::vector<std::pair<int, int>>> preconditions;
    abstract_operators_.reserve(g_operators.size());
    preconditions.reserve(g_operators.size());

    using footprint_t =
        std::vector<std::pair<AbstractState, value_type::value_t>>;

    std::set<footprint_t> operators;

    for (unsigned op_id = 0; op_id < g_operators.size(); ++op_id) {
        const ProbabilisticOperator& op = g_operators[op_id];

        std::vector<int> affected_var_indices;
        std::vector<int> eff_no_pre_var_indices;
        std::vector<int> dense_precondition(variables.size(), -1);
        {
            for (const auto& [var, val] : op.get_preconditions()) {
                const int idx = var_index_[var];
                if (idx != -1) {
                    dense_precondition[idx] = val;
                    affected_var_indices.push_back(idx);
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

                        const int pre_val = dense_precondition[idx];
                        int val_change;

                        if (pre_val == -1) {
                            eff_no_pre_var_indices.push_back(idx);
                            info.missing_pres.push_back(idx);
                            val_change = eff_val;
                        } else {
                            val_change = eff_val - pre_val;
                        }

                        info.base_effect +=
                            state_mapper_->from_value_partial(idx, val_change);
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
            setify(eff_no_pre_var_indices);

            affected_var_indices.insert(
                affected_var_indices.end(),
                eff_no_pre_var_indices.begin(),
                eff_no_pre_var_indices.end());
            std::sort(affected_var_indices.begin(), affected_var_indices.end());

            if (value_type::approx_greater()(self_loop_prob, value_type::zero)) {
                outcomes.emplace_back(self_loop_prob);
            }

            auto add_operator = [this,
                                 op,
                                 op_id,
                                 &operators,
                                 &preconditions,
                                 &outcomes,
                                 &affected_var_indices](const std::vector<int>& values)
            {
                footprint_t footprint;

                AbstractOperator new_op(op_id);
                new_op.cost = op.get_cost();
                new_op.pre =
                    state_mapper_->from_values_partial(affected_var_indices, values);
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
                    auto a = state_mapper_->from_values_partial(missing_pres, values);
                    new_op.outcomes.add(base_effect - a, probability);
                }

                assert(value_type::approx_equal()(sum, value_type::one));

                for (const auto& out : new_op.outcomes) {
                    footprint.emplace_back(out.first, out.second);
                }

                std::sort(footprint.begin(), footprint.end());
                
                for (int pre_var_index : affected_var_indices) {
                    footprint.emplace_back(pre_var_index, values[pre_var_index]);
                }

                if (operators.insert(footprint).second) {
                    abstract_operators_.emplace_back(std::move(new_op));
                    preconditions.emplace_back(affected_var_indices.size());
                    auto& precons = preconditions.back();
                    for (size_t j = 0; j < affected_var_indices.size(); ++j) {
                        const int idx = affected_var_indices[j];
                        precons[j] = std::make_pair(idx, values[idx]);
                    }
                }
            };

            state_mapper_->enumerate(
                dense_precondition, eff_no_pre_var_indices, add_operator);
        }
    }

    std::vector<const AbstractOperator*> opptrs(abstract_operators_.size());
    for (size_t i = 0; i < abstract_operators_.size(); ++i) {
        opptrs[i] = &abstract_operators_[i];
    }

    assert(abstract_operators_.size() == preconditions.size());

    progression_aops_generator_ =
        std::make_shared<ProgressionSuccessorGenerator>(
            state_mapper_->get_domains(), preconditions, opptrs);
}

void ProbabilisticProjection::add_to_goals(AbstractState state) {
    goal_states_.insert(state);
}

} // namespace pdbs
} // namespace probabilistic
