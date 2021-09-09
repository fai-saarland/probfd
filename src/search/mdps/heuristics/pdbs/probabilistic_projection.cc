#include "probabilistic_projection.h"

#include "../../../global_operator.h"
#include "../../../successor_generator.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../globals.h"

#include <algorithm>
#include <deque>
#include <numeric>
#include <set>

namespace probabilistic {
namespace pdbs {

struct NoGoalVariableException : std::exception {
    const char* what() const noexcept override
    {
        return "Construction of a PDB without a goal "
               "variable is intentionally disallowed!";
    }
};

ProbabilisticProjection::ProbabilisticProjection(
    const Pattern& pattern,
    const std::vector<int>& domains)
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

unsigned int ProbabilisticProjection::num_states() const
{
    return state_mapper_->size();
}

AbstractState
ProbabilisticProjection::get_abstract_state(const GlobalState& s) const
{
    return state_mapper_->operator()(s);
}

const Pattern& ProbabilisticProjection::get_pattern() const
{
    return state_mapper_->get_pattern();
}

unsigned int ProbabilisticProjection::num_reachable_states() const
{
    return reachable_states;
}

void ProbabilisticProjection::prepare_regression()
{
    std::vector<std::vector<std::pair<int, int>>> progressions;
    progressions.reserve(::g_operators.size());
    std::vector<AbstractState> operators;

    using footprint_t =
        std::pair<std::vector<std::pair<int, int>>, AbstractState>;
    utils::HashSet<footprint_t> operator_set;

    for (const auto& op : ::g_operators) {
        std::vector<std::pair<int, int>> after_effect;
        std::vector<int> eff_no_pre;

        std::unordered_map<int, int> precondition;
        std::unordered_map<int, int> effect;
        {
            for (const auto [pre_var, pre_val] : op.get_preconditions()) {
                const int idx = var_index_[pre_var];
                if (idx != -1) {
                    precondition[idx] = pre_val;
                }
            }

            for (const auto [eff_var, eff_val, _] : op.get_effects()) {
                const int idx = var_index_[eff_var];
                if (idx != -1) {
                    effect[idx] = eff_val;
                }
            }
        }

        if (effect.empty()) {
            continue;
        }

        AbstractState pre(0);
        AbstractState eff(0);
        {
            for (const auto [pre_var, pre_val] : precondition) {
                if (effect.find(pre_var) == effect.end()) {
                    after_effect.emplace_back(pre_var, pre_val);
                }
            }

            for (const auto [eff_var, eff_val] : effect) {
                after_effect.emplace_back(eff_var, eff_val);
                eff += state_mapper_->from_value_partial(eff_var, eff_val);
                if (precondition.find(eff_var) == precondition.end()) {
                    eff_no_pre.push_back(eff_var);
                } else {
                    pre += state_mapper_->from_value_partial(
                        eff_var,
                        precondition[eff_var]);
                }
            }
        }

        std::sort(after_effect.begin(), after_effect.end());
        std::sort(eff_no_pre.begin(), eff_no_pre.end());

        auto it = state_mapper_->partial_states_begin(
            pre - eff,
            std::move(eff_no_pre));
        auto end = state_mapper_->partial_states_end();

        for (; it != end; ++it) {
            AbstractState regression = *it;

            if (regression.id != 0 &&
                operator_set.emplace(after_effect, regression).second) {
                progressions.push_back(after_effect);
                operators.push_back(regression);
            }
        }
    }

    regression_aops_generator_ = std::make_shared<RegressionSuccessorGenerator>(
        state_mapper_->get_domains(),
        progressions,
        operators);
}

bool is_closed(
    AbstractState state,
    const AbstractOperator* op,
    const std::unordered_set<AbstractState>& closure)
{
    for (auto outcome : op->outcomes) {
        auto successor = state + outcome.first;
        if (closure.find(successor) == closure.end()) {
            return false;
        }
    }

    return true;
}

bool has_closed_operator(
    AbstractState state,
    std::set<const probabilistic::pdbs::AbstractOperator*>& operators,
    const std::unordered_set<AbstractState>& closure)
{
    for (const auto* op : operators) {
        if (is_closed(state, op, closure)) {
            return true;
        }
    }

    return false;
}

void ProbabilisticProjection::precompute_dead_ends()
{
    // Initialize open list with goal states.
    std::deque<AbstractState> open(goal_states_.begin(), goal_states_.end());

    std::unordered_set<AbstractState> non_dead_ends(
        goal_states_.begin(),
        goal_states_.end());

    // Regress from goal states to find all states with MaxProb > 0.
    while (!open.empty()) {
        const AbstractState s = open.front();
        open.pop_front();

        std::vector<int> state_values;
        state_mapper_->to_values(s, state_values);

        std::vector<AbstractState> aops;
        regression_aops_generator_->generate_applicable_ops(state_values, aops);
        if (aops.empty()) {
            continue;
        }

        for (const AbstractState eff : aops) {
            const AbstractState t = s + eff;

            // Collect operators of t reaching s
            std::vector<int> facts = state_mapper_->to_values(t);
            std::vector<const probabilistic::pdbs::AbstractOperator*> aops;
            progression_aops_generator_->generate_applicable_ops(facts, aops);

            if (non_dead_ends.find(t) == non_dead_ends.end()) {
                non_dead_ends.insert(t);
                open.push_back(t);
            }
        }
    }

    dead_ends = QualitativeResultStore(true, non_dead_ends);

    // Compute probability one states.
    class std::unordered_set<probabilistic::pdbs::AbstractState>& S_new =
        non_dead_ends;
    class std::unordered_set<probabilistic::pdbs::AbstractState> S_old;

    do {
        S_old = std::move(S_new);
        S_new.insert(goal_states_.begin(), goal_states_.end());

        std::deque<AbstractState> open(
            goal_states_.begin(),
            goal_states_.end());

        while (!open.empty()) {
            const AbstractState s = open.front();
            open.pop_front();

            std::vector<int> state_values;
            state_mapper_->to_values(s, state_values);

            std::vector<AbstractState> aops;
            regression_aops_generator_->generate_applicable_ops(
                state_values,
                aops);

            if (aops.empty()) {
                continue;
            }

            for (const AbstractState eff : aops) {
                const AbstractState t = s + eff;

                if (S_old.find(t) == S_old.end()) {
                    continue;
                }

                // Collect operators of t reaching s
                std::vector<int> facts = state_mapper_->to_values(t);
                std::vector<const probabilistic::pdbs::AbstractOperator*> aops;
                progression_aops_generator_->generate_applicable_ops(
                    facts,
                    aops);

                // Check if there is an inverse operator that reaches s from t
                // and stays within the current set. If yes, add t.
                for (const auto* op : aops) {
                    bool inverse = false;
                    bool closed = true;

                    for (auto out : op->outcomes) {
                        auto succ = t + out.first;
                        if (succ == s) {
                            inverse = true;
                        }

                        if (S_old.find(succ) == S_old.end()) {
                            closed = false;
                            break;
                        }
                    }

                    if (inverse && closed && S_new.find(t) == S_new.end()) {
                        S_new.insert(t);
                        open.push_back(t);
                        break;
                    }
                }
            }
        }
    } while (S_old.size() != S_new.size()); // Size check is enough here

    this->one_states = QualitativeResultStore(false, S_old);
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
        base_goal,
        std::move(non_goal_vars));
    auto end = state_mapper_->partial_states_end();

    for (; it != end; ++it) {
        goal_states_.insert(*it);
    }
}

void ProbabilisticProjection::prepare_progression()
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
            state_mapper_->get_domains(),
            preconditions,
            opptrs);
}

struct OutcomeInfo {
    AbstractState base_effect = AbstractState(0);
    std::vector<int> missing_pres;

    friend bool operator<(const OutcomeInfo& a, const OutcomeInfo& b)
    {
        return std::tie(a.base_effect, a.missing_pres) <
               std::tie(b.base_effect, b.missing_pres);
    }

    friend bool operator==(const OutcomeInfo& a, const OutcomeInfo& b)
    {
        return std::tie(a.base_effect, a.missing_pres) ==
               std::tie(b.base_effect, b.missing_pres);
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
                    utils::insert_set(affected_var_indices, idx);
                    info.missing_pres.push_back(idx);
                    val_change = eff_val;
                } else {
                    val_change = eff_val - pre_val;
                }

                info.base_effect +=
                    state_mapper_->from_value_partial(idx, val_change);
            }
        }

        if (self_loop) {
            self_loop_prob += prob;
        } else {
            outcomes.add(std::move(info), prob);
        }
    }

    utils::sort_unique(eff_no_pre_var_indices);
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
        std::move(dense_precondition),
        std::move(eff_no_pre_var_indices));
    auto end = state_mapper_->cartesian_subsets_end();

    for (; it != end; ++it) {
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
            affected_var_indices,
            values);

        if (duplicate_set.emplace(pre_hash, new_op).second) {
            abstract_operators_.emplace_back(std::move(new_op));
            auto& precons =
                preconditions.emplace_back(affected_var_indices.size());

            for (size_t j = 0; j < affected_var_indices.size(); ++j) {
                const int idx = affected_var_indices[j];
                precons[j] = std::make_pair(idx, values[idx]);
            }
        }
    };
}

} // namespace pdbs
} // namespace probabilistic
