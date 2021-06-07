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

template <typename T, typename A>
void make_set(std::vector<T, A>& vector) {
    std::sort(vector.begin(), vector.end());
    auto it = std::unique(vector.begin(), vector.end());
    vector.erase(it, vector.end());
}

// TODO move to utils
template <typename T, typename A>
void insert_set_sorted(std::vector<T, A>& vector, T elem) {
    assert(std::is_sorted(vector.begin(), vector.end()));

    auto it = std::lower_bound(vector.begin(), vector.end(), elem);

    if (it == vector.end() || *it != elem) {
        vector.insert(it, std::move(elem));
    }
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

    auto it = state_mapper_->partial_states_begin(
        base_goal, std::move(non_goal_vars));
    auto end = state_mapper_->partial_states_end();

    for (; it != end; ++it) {
        goal_states_.insert(*it);
    }
}

void
ProbabilisticProjection::prepare_progression()
{
    abstract_operators_.reserve(g_operators.size());

    const Pattern& pattern = state_mapper_->get_pattern();

    std::vector<std::vector<std::pair<int, int>>> preconditions;
    preconditions.reserve(g_operators.size());

    std::set<ProgressionOperatorFootprint> duplicate_set;

    // Generate the abstract operators for each probabilistic operator
    for (const ProbabilisticOperator& op : g_operators) {
        add_abstract_operators(pattern, op, duplicate_set, preconditions);
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

struct OutcomeInfo {
    AbstractState base_effect = AbstractState(0);
    std::vector<int> missing_pres;

    friend bool operator<(const OutcomeInfo& a, const OutcomeInfo& b) {
        return a.base_effect < b.base_effect;
    }

    friend bool operator==(const OutcomeInfo& a, const OutcomeInfo& b) {
        return a.base_effect == b.base_effect;
    }
};

void ProbabilisticProjection::add_abstract_operators(
    const Pattern& pattern,
    const ProbabilisticOperator& op,
    std::set<ProgressionOperatorFootprint>& duplicate_set,
    std::vector<std::vector<std::pair<int, int>>>& preconditions)
{
    const int operator_id = op.get_id();
    const int cost = op.get_cost();

    // The affected variables (pre + eff)
    std::vector<int> affected_var_indices;

    // The precondition embedded into an abstract state vector
    std::vector<int> dense_precondition(pattern.size(), -1);
    
    for (const auto [var, val] : op.get_preconditions()) {
        const int idx = var_index_[var];
        if (idx != -1) {
            dense_precondition[idx] = val;
            affected_var_indices.push_back(idx);
        }
    }

    // Info about each probabilistic outcome
    Distribution<OutcomeInfo> outcomes;

    // Probability for no effect
    value_type::value_t self_loop_prob = value_type::zero;

    // Variables that appear in effects but not in the precondition
    std::vector<int> eff_no_pre_var_indices;

    // Collect info about the outcomes
    for (const ProbabilisticOutcome& out : op) {
        const std::vector<GlobalEffect>& effects = out.op->get_effects();
        const value_type::value_t prob = out.prob;

        bool self_loop = true;
        OutcomeInfo info;

        for (const auto& [eff_var, eff_val, _] : effects) {
            const int idx = var_index_[eff_var];

            if (idx != -1) {
                self_loop = false;

                const int pre_val = dense_precondition[idx];
                int val_change;

                if (pre_val == -1) {
                    eff_no_pre_var_indices.push_back(idx);
                    insert_set_sorted(affected_var_indices, idx);
                    info.missing_pres.push_back(idx);
                    val_change = eff_val;
                } else {
                    val_change = eff_val - pre_val;
                }

                info.base_effect += state_mapper_->from_value_partial(idx, val_change);
            }
        }

        if (self_loop) {
            self_loop_prob += prob;
        } else {
            outcomes.add(std::move(info), prob);
        }
    }

    make_set(eff_no_pre_var_indices);
    outcomes.make_unique();

    if (value_type::approx_greater()(self_loop_prob, value_type::zero)) {
        outcomes.add(OutcomeInfo(), self_loop_prob);
    }

    // We need to enumerate all values for variables that are not part of
    // the precondition but in an effect. Depending on the value of the
    // variable, the value change caused by the abstract operator would be
    // different, hence we generate on operator for each state where enabled.

    // Variables in the precondition need not be enumerated, their value
    // change is always effect value minus precondition value.

    auto it = state_mapper_->cartesian_subsets_begin(
        std::move(dense_precondition), std::move(eff_no_pre_var_indices));
    auto end = state_mapper_->cartesian_subsets_end();

    for (; it != end; ++it)
    {
        const std::vector<int>& values = *it;

        AbstractOperator new_op(operator_id, cost);
#ifndef NDEBUG
        value_type::value_t sum = value_type::zero;
#endif
        for (const auto& [info, prob] : outcomes) {
            const auto& [base_effect, missing_pres] = info;
#ifndef NDEBUG
            assert(value_type::approx_greater()(prob, value_type::zero));
            sum += prob;
#endif
            auto a = state_mapper_->from_values_partial(missing_pres, values);
            new_op.outcomes.add(base_effect - a, prob);
        }

        assert(value_type::approx_equal()(sum, value_type::one));

        new_op.outcomes.make_unique();

        int pre_hash = state_mapper_->get_unique_partial_state_id(
            affected_var_indices, values);

        if (duplicate_set.emplace(pre_hash, new_op).second) {
            abstract_operators_.emplace_back(std::move(new_op));
            auto& precons = preconditions.emplace_back(affected_var_indices.size());

            for (size_t j = 0; j < affected_var_indices.size(); ++j) {
                const int idx = affected_var_indices[j];
                precons[j] = std::make_pair(idx, values[idx]);
            }
        }
    };
}

} // namespace pdbs
} // namespace probabilistic
