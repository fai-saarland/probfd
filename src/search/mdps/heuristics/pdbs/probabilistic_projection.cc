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
    build_operators();
    compute_dead_ends();
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

unsigned int ProbabilisticProjection::num_dead_ends() const
{
    return state_mapper_->size() - dead_ends->get_storage().size();
}

unsigned int ProbabilisticProjection::num_proper_states() const
{
    assert(proper_states);
    return proper_states->get_storage().size();
}

unsigned int ProbabilisticProjection::num_reachable_states() const
{
    return reachable_states;
}

bool ProbabilisticProjection::has_only_proper_states() const
{
    return state_mapper_->size() == proper_states->get_storage().size();
}

bool ProbabilisticProjection::has_only_dead_or_proper_states() const
{
    return dead_ends->get_storage().size() ==
           proper_states->get_storage().size();
}

bool ProbabilisticProjection::is_dead_end(const GlobalState& s) const
{
    return is_dead_end(get_abstract_state(s));
}

bool ProbabilisticProjection::is_dead_end(const AbstractState& s) const
{
    return dead_ends->get(s);
}

AbstractState
ProbabilisticProjection::get_abstract_state(const GlobalState& s) const
{
    return state_mapper_->operator()(s);
}

AbstractState
ProbabilisticProjection::get_abstract_state(const std::vector<int>& s) const
{
    return state_mapper_->operator()(s);
}

const Pattern& ProbabilisticProjection::get_pattern() const
{
    return state_mapper_->get_pattern();
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

void ProbabilisticProjection::compute_dead_ends()
{
    if (dead_ends) {
        return;
    }

    // Initialize open list with goal states.
    std::deque<AbstractState> open(goal_states_.begin(), goal_states_.end());

    std::unordered_set<AbstractState> closed(
        goal_states_.begin(),
        goal_states_.end());

    // Goal distance for every state
    std::vector<int> goal_distances(state_mapper_->size(), -1);

    for (AbstractState goal : goal_states_) {
        goal_distances[goal.id] = 0;
    }

    // Regression breadth-first search from goal states to find all states with
    // MaxProb > 0. While we are at it, we can compute the goal distances.
    while (!open.empty()) {
        const AbstractState s = open.front();
        open.pop_front();

        std::vector<int> state_values;
        state_mapper_->to_values(s, state_values);

        std::vector<AbstractRegressionOperator> aops;
        regression_aops_generator_->generate_applicable_ops(state_values, aops);
        if (aops.empty()) {
            continue;
        }

        for (const AbstractRegressionOperator& regr_op : aops) {
            const AbstractState t = s + regr_op.effect;

            if (closed.find(t) == closed.end()) {
                closed.insert(t);
                open.push_back(t);
            }
        }
    }

    dead_ends.reset(new QualitativeResultStore(true, std::move(closed)));
}

void ProbabilisticProjection::compute_proper_states()
{
    if (proper_states) {
        return;
    }

    compute_dead_ends();

    // Compute probability one states.
    class std::unordered_set<AbstractState> S_new = dead_ends->get_storage();
    class std::unordered_set<AbstractState> S_old;

    if (S_new.size() == this->state_mapper_->size()) {
        proper_states.reset(
            new QualitativeResultStore(false, std::move(S_new)));
        return;
    }

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

            std::vector<AbstractRegressionOperator> aops;
            regression_aops_generator_->generate_applicable_ops(
                state_values,
                aops);

            if (aops.empty()) {
                continue;
            }

            for (const AbstractRegressionOperator& regr_op : aops) {
                const AbstractState t = s + regr_op.effect;

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
                                            // (monotonically decreasing)

    proper_states.reset(new QualitativeResultStore(false, std::move(S_old)));
}

void ProbabilisticProjection::setup_abstract_goal()
{
    const Pattern& variables = state_mapper_->get_pattern();
    std::vector<int> non_goal_vars;

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    int v = 0;
    for (const auto& [var, val] : g_goal) {
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

    auto goals =
        state_mapper_->partial_states(base_goal, std::move(non_goal_vars));

    for (const AbstractState& g : goals) {
        goal_states_.insert(g);
    }
}

void ProbabilisticProjection::build_operators()
{
    abstract_operators_.reserve(g_operators.size());

    std::vector<AbstractRegressionOperator> abstract_regression_operators;
    abstract_regression_operators.reserve(g_operators.size());

    std::vector<std::vector<std::pair<int, int>>> progression_preconditions;
    std::vector<std::vector<std::pair<int, int>>> regression_preconditions;

    progression_preconditions.reserve(g_operators.size());
    regression_preconditions.reserve(g_operators.size());

    std::set<ProgressionOperatorFootprint> duplicate_set;

    // Generate the abstract operators for each probabilistic operator
    for (const ProbabilisticOperator& op : g_operators) {
        add_abstract_operators(
            op,
            duplicate_set,
            abstract_regression_operators,
            progression_preconditions,
            regression_preconditions);
    }

    std::vector<const AbstractOperator*> opptrs(abstract_operators_.size());
    for (size_t i = 0; i < abstract_operators_.size(); ++i) {
        opptrs[i] = &abstract_operators_[i];
    }

    assert(abstract_operators_.size() == progression_preconditions.size());
    assert(
        abstract_regression_operators.size() ==
        regression_preconditions.size());

    // Build applicable operator generators
    progression_aops_generator_ =
        std::make_shared<ProgressionSuccessorGenerator>(
            state_mapper_->get_domains(),
            progression_preconditions,
            opptrs);

    regression_aops_generator_ = std::make_shared<RegressionSuccessorGenerator>(
        state_mapper_->get_domains(),
        regression_preconditions,
        std::move(abstract_regression_operators));
}

namespace {
struct OutcomeInfo {
    AbstractState base_effect = AbstractState(0);
    std::vector<int> missing_pres;
    std::vector<std::pair<int, int>> effects;

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
} // namespace

void ProbabilisticProjection::add_abstract_operators(
    const ProbabilisticOperator& op,
    std::set<ProgressionOperatorFootprint>& duplicate_set,
    std::vector<AbstractRegressionOperator>& regression_operators,
    std::vector<std::vector<std::pair<int, int>>>& progression_preconditions,
    std::vector<std::vector<std::pair<int, int>>>& regression_preconditions)
{
    const int operator_id = op.get_id();
    const int cost = op.get_cost();

    // The affected variables (pre + eff)
    std::vector<int> affected_var_indices;

    // The precondition embedded into an abstract state vector
    std::vector<int> dense_precondition(
        state_mapper_->get_pattern().size(),
        -1);

    for (const auto& [var, val] : op.get_preconditions()) {
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

        OutcomeInfo info;

        for (const auto& [eff_var, eff_val, _] : effects) {
            const int idx = var_index_[eff_var];

            if (idx != -1) {
                info.effects.emplace_back(idx, eff_val);

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

        if (info.effects.empty()) {
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

    auto ran = state_mapper_->cartesian_subsets(
        std::move(dense_precondition),
        std::move(eff_no_pre_var_indices));

    for (const std::vector<int>& values : ran) {
        AbstractOperator new_op(operator_id, cost);
        std::vector<AbstractRegressionOperator> reg_ops;

        std::vector<std::vector<std::pair<int, int>>> effects;

        for (const auto& [info, prob] : outcomes) {
            const auto& [base_effect, missing_pres, effect] = info;
            auto a = state_mapper_->from_values_partial(missing_pres, values);
            const AbstractState change = base_effect - a;
            auto it = new_op.outcomes.find(change);

            if (it != new_op.outcomes.end()) {
                it->second += prob;
            } else {
                new_op.outcomes.add(change, prob);
                effects.push_back(effect);
            }
        }

        // new_op.outcomes.make_unique();

        int pre_hash = state_mapper_->get_unique_partial_state_id(
            affected_var_indices,
            values);

        // Duplicate pruning
        if (!duplicate_set.emplace(pre_hash, new_op).second) {
            continue;
        }

        // Add the progression and regression operator
        const size_t pid = progression_preconditions.size();

        auto& abs_op = abstract_operators_.emplace_back(std::move(new_op));
        auto& precondition =
            progression_preconditions.emplace_back(affected_var_indices.size());

        for (size_t j = 0; j < affected_var_indices.size(); ++j) {
            const int idx = affected_var_indices[j];
            precondition[j] = std::make_pair(idx, values[idx]);
        }

        // Generate regression operators
        for (size_t i = 0; i != abs_op.outcomes.size(); ++i) {
            const AbstractState change = abs_op.outcomes.data()[i].first;
            const auto& effect = effects[i];

            // Apply effect to progression precondition to get regression
            // precondition
            std::vector<std::pair<int, int>> regression_precondition =
                precondition;

            for (const auto& [eff_var, eff_val] : effect) {
                int i = 0;

                while (regression_precondition[i].first != eff_var) {
                    ++i;
                }

                regression_precondition[i].second = eff_val;
            }

            // Add regression operators
            regression_preconditions.push_back(
                std::move(regression_precondition));

            regression_operators.emplace_back(pid, AbstractState(-change.id));
        }
    };
}

} // namespace pdbs
} // namespace probabilistic
