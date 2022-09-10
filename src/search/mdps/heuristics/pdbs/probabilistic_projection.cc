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
    const std::vector<int>& domains,
    bool operator_pruning)
    : ProbabilisticProjection(
        new AbstractStateMapper(pattern, domains),
        operator_pruning)
{
}

ProbabilisticProjection::ProbabilisticProjection(
    AbstractStateMapper* mapper,
    bool operator_pruning)
    : var_index_(::g_variable_domain.size(), -1)
    , state_mapper_(mapper)
    , initial_state_((*state_mapper_)(::g_initial_state_data))
{
    for (size_t i = 0; i < mapper->get_pattern().size(); ++i) {
        var_index_[mapper->get_pattern()[i]] = i;
    }

    setup_abstract_goal();
    build_operators(operator_pruning);
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
    return state_mapper_->num_states();
}

unsigned int ProbabilisticProjection::num_dead_ends() const
{
    return state_mapper_->num_states() - dead_ends_.size();
}

unsigned int ProbabilisticProjection::num_proper_states() const
{
    return proper_states_.size();
}

unsigned int ProbabilisticProjection::num_reachable_states() const
{
    return reachable_states;
}

bool ProbabilisticProjection::has_only_proper_states() const
{
    return state_mapper_->num_states() == proper_states_.size();
}

bool ProbabilisticProjection::has_only_dead_or_proper_states() const
{
    return dead_ends_.size() == proper_states_.size();
}

bool ProbabilisticProjection::is_dead_end(const GlobalState& s) const
{
    return is_dead_end(get_abstract_state(s));
}

bool ProbabilisticProjection::is_dead_end(const AbstractState& s) const
{
    return utils::contains(dead_ends_, StateID(s.id));
}

bool ProbabilisticProjection::is_goal(const AbstractState& s) const
{
    return utils::contains(goal_states_, s);
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

void ProbabilisticProjection::build_operators(bool operator_pruning)
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
            regression_preconditions,
            operator_pruning);
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
            state_mapper_->domains_begin(),
            progression_preconditions,
            opptrs);

    regression_aops_generator_ = std::make_shared<RegressionSuccessorGenerator>(
        state_mapper_->domains_begin(),
        regression_preconditions,
        std::move(abstract_regression_operators));
}

namespace {
struct OutcomeInfo {
    AbstractState base_effect = AbstractState(0);
    std::vector<int> missing_pres;
    std::vector<std::pair<int, int>> effects;

    friend bool operator==(const OutcomeInfo& a, const OutcomeInfo& b)
    {
        return std::tie(a.base_effect, a.missing_pres) ==
               std::tie(b.base_effect, b.missing_pres);
    }
};
} // namespace

namespace {
auto find_sorted(const std::vector<std::pair<int, int>>& p, int idx)
{
    auto it =
        std::lower_bound(p.begin(), p.end(), idx, [](const auto& a, int i) {
            return a.first < i;
        });
    return it != p.end() && it->first == idx ? it : p.end();
}

void apply(
    std::vector<std::pair<int, int>>& pstate,
    const std::vector<std::pair<int, int>>& effect)
{
    auto it = effect.begin();
    auto end = effect.end();

    auto pit = pstate.begin();
    auto pend = pstate.end();

    for (; it != end; ++it, ++pit) {
        const auto& [idx, val] = *it;

        pit = std::find_if(pit, pend, [idx=idx](const auto& p) {
            return p.first == idx;
        });

        assert(pit != pend);
        pit->second = val;
    }
}
} // namespace

void ProbabilisticProjection::add_abstract_operators(
    const ProbabilisticOperator& op,
    std::set<ProgressionOperatorFootprint>& duplicate_set,
    std::vector<AbstractRegressionOperator>& regression_operators,
    std::vector<std::vector<std::pair<int, int>>>& progression_preconditions,
    std::vector<std::vector<std::pair<int, int>>>& regression_preconditions,
    bool pruning)
{
    const int operator_id = op.get_id();
    const int cost = op.get_cost();

    // Precondition partial state and partial state to enumerate
    // effect values not appearing in precondition
    std::vector<std::pair<int, int>> local_precondition;
    std::vector<std::pair<int, int>> effects_not_in_pre;

    for (const auto& [var, val] : op.get_preconditions()) {
        const int idx = var_index_[var];
        if (idx != -1) {
            local_precondition.emplace_back(idx, val);
        }
    }

    // Info about each probabilistic outcome
    Distribution<OutcomeInfo> outcomes;

    // Collect info about the outcomes
    for (const ProbabilisticOutcome& out : op) {
        const std::vector<GlobalEffect>& effects = out.op->get_effects();
        const value_type::value_t prob = out.prob;

        OutcomeInfo info;

        for (const auto& [eff_var, eff_val, _] : effects) {
            const int idx = var_index_[eff_var];

            if (idx != -1) {
                info.effects.emplace_back(idx, eff_val);

                auto pre_it = find_sorted(local_precondition, idx);
                int val_change;

                if (pre_it == local_precondition.end()) {
                    effects_not_in_pre.emplace_back(idx, 0);
                    info.missing_pres.push_back(idx);
                    val_change = eff_val;
                } else {
                    val_change = eff_val - pre_it->second;
                }

                info.base_effect +=
                    state_mapper_->from_value_partial(idx, val_change);
            }
        }

        outcomes.add_unique(std::move(info), prob);
    }

    utils::sort_unique(effects_not_in_pre);

    // We enumerate all values for variables that are not part of
    // the precondition but in an effect. Depending on the value of the
    // variable, the value change caused by the abstract operator would be
    // different, hence we generate on operator for each state where enabled.
    auto ran = state_mapper_->cartesian_subsets(std::move(effects_not_in_pre));

    for (const std::vector<std::pair<int, int>>& values : ran) {
        // Generate the progression operator
        AbstractOperator new_op(operator_id, cost);

        // Effects are cached for the regression operator generation
        std::vector<std::vector<std::pair<int, int>>> effects;

        for (const auto& [info, prob] : outcomes) {
            const auto& [base_effect, missing_pres, effect] = info;
            auto a = state_mapper_->from_values_partial(missing_pres, values);
            const AbstractState change = base_effect - a;

            if (new_op.outcomes.add_unique(change, prob).second) {
                effects.push_back(effect);
            }
        }

        // Construct the precondition by merging the original precondition
        // partial state with the partial state for the non-precondition effects
        // of this iteration
        std::vector<std::pair<int, int>> precondition;
        precondition.reserve(local_precondition.size() + values.size());

        std::merge(
            local_precondition.begin(),
            local_precondition.end(),
            values.begin(),
            values.end(),
            std::back_inserter(precondition));

        // Generate a hash for the precondition to check for duplicates
        int pre_hash = state_mapper_->get_unique_partial_state_id(precondition);

        if (pruning && !duplicate_set.emplace(pre_hash, new_op).second) {
            continue;
        }

        // Generate and add the regression operators
        const size_t pid = progression_preconditions.size();

        for (size_t i = 0; i != new_op.outcomes.size(); ++i) {
            const AbstractState change = new_op.outcomes.data()[i].first;
            const auto& effect = effects[i];

            // Apply effect to progression precondition. The result is the
            // regression precondition
            apply(regression_preconditions.emplace_back(precondition), effect);

            // Effect is inverted
            regression_operators.emplace_back(pid, AbstractState(-change.id));
        }

        // Now add the progression operators. In this order, we can move
        // construct
        progression_preconditions.push_back(std::move(precondition));
        abstract_operators_.push_back(std::move(new_op));
    };
}

} // namespace pdbs
} // namespace probabilistic
