#include "probabilistic_projection.h"

#include "../../../global_operator.h"
#include "../../../globals.h"
#include "../../../successor_generator.h"
#include "../../../utils/hash.h"
#include "../../../utils/system.h"
#include "../../algorithms/types_storage.h"
#include "../../analysis_objective.h"
#include "../../globals.h"
#include "../../goal_probability_objective.h"
#include "../../logging.h"
#include "../../probabilistic_operator.h"
#include "algorithms.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <set>
#include <sstream>
#include <unordered_map>

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
    for (int i = variables.size() - 1; i >= 0; i--) {
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
    for (const auto& p : g_goal) {
        const int idx = var_index_[p.first];
        if (idx != -1) {
            projected_goal_.emplace_back(idx, p.second);
            goal[idx] = p.second;
        }
    }
    std::sort(projected_goal_.begin(), projected_goal_.end());

    AbstractState base_goal = state_mapper_->from_values(goal);
    std::vector<int> missing;
    int v = 0;
    for (unsigned i = 0; i < projected_goal_.size(); i++) {
        while (v < projected_goal_[i].first) {
            num_goal_states = num_goal_states * domains[v];
            missing.push_back(v++);
        }
        v++;
    }
    while (v < static_cast<int>(variables.size())) {
        num_goal_states = num_goal_states * domains[v];
        missing.push_back(v++);
    }

    if (projected_goal_.empty()) {
        goal_states_.negate_all();
    } else if (2 * num_goal_states >= state_mapper_->size()) {
        missing.clear();
        for (int i = 0; i < static_cast<int>(variables.size()); ++i) {
            missing.push_back(i);
        }
        std::fill(goal.begin(), goal.end(), 0);
        base_goal = AbstractState(0);
        for (unsigned i = 0; i < projected_goal_.size(); ++i) {
            int idx = projected_goal_[i].first;
            missing.erase(
                std::lower_bound(missing.begin(), missing.end(), idx));
            for (int val = 0; val < domains[idx]; ++val) {
                if (val != projected_goal_[i].second) {
                    goal[idx] = val;
                    base_goal += state_mapper_->from_value_partial(idx, val);
                    state_mapper_->enumerate(
                        missing,
                        goal,
                        [this, base_goal](
                            AbstractState missing, const std::vector<int>&) {
                            goal_states_.set(base_goal + missing, true);
                        });
                    base_goal -= state_mapper_->from_value_partial(idx, val);
                }
            }
            goal[idx] = projected_goal_[i].second;
            base_goal += state_mapper_->from_value_partial(
                idx, projected_goal_[i].second);
        }
        goal_states_.negate_all();
    } else {
        state_mapper_->enumerate(
            missing,
            goal,
            [this, base_goal](AbstractState missing, const std::vector<int>&) {
                goal_states_.set(base_goal + missing, true);
            });
    }
}

struct OutcomeInfo {
    OutcomeInfo(value_type::value_t prob)
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

    std::vector<int> pre_var_indices;
    std::vector<int> eff_no_pre_var_indices;
    std::vector<int> pre_assignment(variables.size(), -1);

    using footprint_t =
        std::vector<std::pair<AbstractState, value_type::value_t>>;
    struct footprint_less {
        bool operator()(const footprint_t& x, const footprint_t& y) const
        {
            if (x.size() < y.size()) {
                return true;
            } else if (x.size() > y.size()) {
                return false;
            }
            for (int i = x.size() - 1; i >= 0; i--) {
                if (x[i].first < y[i].first) {
                    return true;
                } else if (x[i].first > y[i].first) {
                    return false;
                }
            }
            for (int i = x.size() - 1; i >= 0; i--) {
                if (lt(x[i].second, y[i].second)) {
                    return true;
                } else if (lt(y[i].second, x[i].second)) {
                    return false;
                }
            }
            return false;
        }
        value_type::approx_less lt;
    };
    // struct footprint_equal {
    //    bool operator()(const footprint_t& x, const footprint_t& y) const
    //    {
    //        if (x.size() < y.size()) {
    //            return false;
    //        } else if (x.size() > y.size()) {
    //            return false;
    //        }
    //        for (int i = x.size() - 1; i >= 0; i--) {
    //            if (x[i].first != y[i].first) {
    //                return false;
    //            } else if (!eq(x[i].second, y[i].second)) {
    //                return false;
    //            }
    //        }
    //        return true;
    //    }
    //    value_type::equal eq;
    //};
    std::set<footprint_t, footprint_less> operators;
    footprint_t footprint;

    for (unsigned op_id = 0; op_id < g_operators.size(); op_id++) {
        const ProbabilisticOperator& op = g_operators[op_id];

        pre_var_indices.clear();
        eff_no_pre_var_indices.clear();

        std::fill(pre_assignment.begin(), pre_assignment.end(), -1);
        {
            const std::vector<GlobalCondition>& pre = op.get_preconditions();
            for (int j = pre.size() - 1; j >= 0; j--) {
                const int var = pre[j].var;
                const int val = pre[j].val;
                const int idx = var_index_[var];
                if (idx != -1) {
                    pre_assignment[idx] = val;
                    pre_var_indices.push_back(idx);
                }
            }
        }

        std::vector<OutcomeInfo> outcomes;
        value_type::value_t self_loop_prob = value_type::zero;
        for (auto it = op.begin(); it != op.end(); it++) {
            bool self_loop = true;
            OutcomeInfo info(it->prob);
            {
                const std::vector<GlobalEffect>& effects =
                    it->op->get_effects();
                for (int j = effects.size() - 1; j >= 0; j--) {
                    const int var = effects[j].var;
                    const int idx = var_index_[var];
                    if (idx != -1) {
                        self_loop = false;
                        info.base_effect += state_mapper_->from_value_partial(
                            idx, effects[j].val);
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
                self_loop_prob += it->prob;
            } else {
                outcomes.emplace_back(std::move(info));
            }
        }

        if (outcomes.size() > 0) {
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
                                    const std::vector<int>& values) {
                AbstractOperator new_op(op_id);
                new_op.pre =
                    state_mapper_->from_values_partial(pre_var_indices, values);
#ifndef NDEBUG
                value_type::value_t sum = value_type::zero;
#endif
                for (auto out = outcomes.begin(); out != outcomes.end();
                     out++) {
#ifndef NDEBUG
                    assert(value_type::approx_greater()(
                        out->probability, value_type::zero));
                    sum += out->probability;
#endif
                    new_op.outcomes.add(
                        out->base_effect
                            - state_mapper_->from_values_partial(
                                out->missing_pres, values),
                        out->probability);
                }
                assert(value_type::approx_equal()(sum, value_type::one));

                for (auto out = new_op.outcomes.begin();
                     out != new_op.outcomes.end();
                     out++) {
                    footprint.emplace_back(out->first, out->second);
                }
                std::sort(footprint.begin(), footprint.end());
                footprint.emplace_back(
                    state_mapper_->from_values(values), value_type::zero);
                if (operators.insert(footprint).second) {
                    abstract_operators_.emplace_back(std::move(new_op));
                    preconditions.emplace_back(pre_var_indices.size());
                    auto& precons = preconditions.back();
                    for (int j = pre_var_indices.size() - 1; j >= 0; j--) {
                        const int idx = pre_var_indices[j];
                        precons[j] = std::pair<int, int>(idx, values[idx]);
                    }
                }
                footprint.clear();
            };
            state_mapper_->enumerate(
                eff_no_pre_var_indices, pre_assignment, add_operator);
        }
    }

    std::vector<const AbstractOperator*> opptrs(
        abstract_operators_.size(), nullptr);
    for (int i = abstract_operators_.size() - 1; i >= 0; i--) {
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

        std::vector<int> precondition(pattern.size(), -1);
        std::vector<std::pair<int, int>> projected_eff;
        std::vector<int> eff_no_pre;
        for (unsigned i = 0; i < ::g_operators.size(); i++) {
            const GlobalOperator& op = ::g_operators[i];
            eff_no_pre.clear();
            projected_eff.clear();
            std::fill(precondition.begin(), precondition.end(), -1);
            {
                const auto& all_pres = op.get_preconditions();
                for (int j = all_pres.size() - 1; j >= 0; j--) {
                    const int var = all_pres[j].var;
                    const int idx = var_index_[var];
                    if (idx != -1) {
                        precondition[idx] = all_pres[j].val;
                    }
                }
            }
            AbstractState pre(0);
            AbstractState eff(0);
            {
                const auto& all_effs = op.get_effects();
                for (int j = all_effs.size() - 1; j >= 0; j--) {
                    const int var = all_effs[j].var;
                    const int idx = var_index_[var];
                    if (idx != -1) {
                        projected_eff.emplace_back(idx, all_effs[j].val);
                        eff += state_mapper_->from_value_partial(
                            idx, all_effs[j].val);
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
                [this,
                 &progressions,
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

QualitativeResultStore
ProbabilisticProjection::compute_dead_ends()
{
    QualitativeResultStore result;
    if (projected_goal_.empty()) {
        result.negate_all();
        return result;
    }

    prepare_regression();

    std::deque<AbstractState> open;
    {
        AbstractState partial(0);
        std::vector<int> goal(state_mapper_->get_variables().size(), 0);
        std::vector<int> missing;
        int idx = 0;
        for (unsigned i = 0; i < projected_goal_.size(); ++i) {
            while (idx < projected_goal_[i].first) {
                missing.push_back(idx++);
            }
            partial += state_mapper_->from_value_partial(
                projected_goal_[i].first, projected_goal_[i].second);
            goal[projected_goal_[i].first] = projected_goal_[i].second;
            ++idx;
        }
        while (idx < static_cast<int>(state_mapper_->get_variables().size())) {
            missing.push_back(idx++);
        }
        state_mapper_->enumerate(
            missing,
            goal,
            [&result, &open, &partial](
                AbstractState add, const std::vector<int>&) {
                const AbstractState goal_state = partial + add;
                open.push_back(goal_state);
                result.set(goal_state, true);
            });
    }

    std::vector<AbstractState> aops;
    std::vector<int> state_values;
    while (!open.empty()) {
        const AbstractState s = open.front();
        open.pop_front();
        state_mapper_->to_values(s, state_values);
        regression_aops_generator_->generate_applicable_ops(state_values, aops);
        if (aops.empty()) {
            continue;
        }
        const AbstractState* eff = &aops[0];
        for (int j = aops.size(); j > 0; --j, ++eff) {
            const AbstractState t = s + *eff;
            if (!result.get(t)) {
                result.set(t, true);
                open.push_back(t);
            }
        }
        aops.clear();
    }

    result.negate_all();
    return result;
}

void
ProbabilisticProjection::dump_graphviz(
    AbstractStateEvaluator* state_reward,
    const std::string& path,
    bool show_transition_labels)
{
    setup_abstract_operators();

    algorithms::StateIDMap<AbstractState> state_id_map;
    algorithms::
        ApplicableActionsGenerator<AbstractState, const AbstractOperator*>
            aops_gen(state_mapper_, progression_aops_generator_);
    algorithms::TransitionGenerator<AbstractState, const AbstractOperator*>
        transition_gen;

    AbstractStateToString state_to_string(state_mapper_);
    AbstractOperatorToString op_to_string(&g_operators);

    ProjectionGraphVis<AbstractStateToString> vis(
        &state_id_map,
        &aops_gen,
        &transition_gen,
        state_reward,
        &state_to_string,
        show_transition_labels ? &op_to_string : nullptr,
        true);

    std::ofstream out;
    out.open(path);
    vis(out, initial_state_);
    out.close();
}

void
ProbabilisticProjection::dump_graphviz(
    AbstractStateEvaluator* state_reward,
    AbstractAnalysisResult* values,
    value_type::value_t v0,
    value_type::value_t v1,
    const std::string& path,
    bool show_transition_labels)
{
    setup_abstract_operators();

    algorithms::StateIDMap<AbstractState> state_id_map;
    algorithms::
        ApplicableActionsGenerator<AbstractState, const AbstractOperator*>
            aops_gen(state_mapper_, progression_aops_generator_);
    algorithms::TransitionGenerator<AbstractState, const AbstractOperator*>
        transition_gen;

    struct StateToString {
        explicit StateToString(
            AbstractAnalysisResult* values,
            value_type::value_t v0,
            value_type::value_t v1,
            std::shared_ptr<AbstractStateMapper> state_mapper)
            : values(values)
            , v0(v0)
            , v1(v1)
            , state_str(state_mapper)
        {
        }

        std::string operator()(const AbstractState& x)
        {
            std::ostringstream out;
            out << state_str(x) << " {";
            if (values->dead_ends->get(x)) {
                out << "dead:" << v0;
            } else if (
                values->one_states != nullptr && values->one_states->get(x)) {
                out << "one:" << v1;
            } else {
                out << values->value_table->get(x);
            }
            out << "}";
            return out.str();
        }

        AbstractAnalysisResult* values;
        value_type::value_t v0;
        value_type::value_t v1;
        AbstractStateToString state_str;
    };

    StateToString state_to_string(values, v0, v1, state_mapper_);
    AbstractOperatorToString op_to_string(&g_operators);

    ProjectionGraphVis<StateToString> vis(
        &state_id_map,
        &aops_gen,
        &transition_gen,
        state_reward,
        &state_to_string,
        show_transition_labels ? &op_to_string : nullptr,
        true);

    std::ofstream out;
    out.open(path);
    vis(out, initial_state_);
    out.close();
}

AbstractAnalysisResult
ProbabilisticProjection::compute_value_table(
    AbstractStateEvaluator* state_reward,
    AbstractTransitionRewardFunction* transition_reward,
    value_type::value_t dead_end_value,
    value_type::value_t upper,
    bool one,
    value_type::value_t one_state_reward,
    QualitativeResultStore* dead_ends)
{
    setup_abstract_operators();

    algorithms::StateIDMap<AbstractState> state_id_map;
    algorithms::
        ApplicableActionsGenerator<AbstractState, const AbstractOperator*>
            aops_gen(state_mapper_, progression_aops_generator_);
    algorithms::TransitionGenerator<AbstractState, const AbstractOperator*>
        transition_gen;

    std::unique_ptr<AbstractStateEvaluator> heuristic = nullptr;
    if (dead_ends != nullptr) {
        heuristic =
            std::unique_ptr<AbstractStateEvaluator>(new AbstractStateInStore(
                dead_ends, value_type::zero, value_type::zero));
    }

    IntervalIteration vi(
        &state_id_map,
        &aops_gen,
        &transition_gen,
        state_reward,
        transition_reward,
        dead_end_value,
        upper,
        heuristic.get(),
        one,
        one_state_reward);

    IntervalIteration::ValueStore values;
    IntervalIteration::BoolStore deads(false); // states that cannot reach goal
    // states that can reach goal with absolute certainty
    IntervalIteration::BoolStore ones(false);

    vi.solve(initial_state_, &values, &deads, &ones);

#if !defined(NDEBUG)
    {
        const StateID state_id = state_id_map[initial_state_];
        g_debug << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_variables().size(); ++i) {
            g_debug << (i > 0 ? ", " : "") << state_mapper_->get_variables()[i];
        }
        g_debug
            << "]: lb="
            << algorithms::interval_iteration::lower_bound(values[state_id])
            << ", ub="
            << algorithms::interval_iteration::upper_bound(values[state_id])
            << ", error="
            << (algorithms::interval_iteration::upper_bound(values[state_id])
                - algorithms::interval_iteration::lower_bound(values[state_id]))
            << std::endl;
    }
#endif

    AbstractAnalysisResult result;
    result.reachable_states = state_id_map.size();
    result.value_table = new QuantitativeResultStore();
    if (dead_ends == nullptr) {
        result.dead_ends = new QualitativeResultStore();
    }
    if (one) {
        result.one_states = new QualitativeResultStore();
        result.one_state_reward = one_state_reward;
    }

    const bool store_deads = dead_ends == nullptr;
    for (auto it = state_id_map.indirection_begin();
         it != state_id_map.indirection_end();
         ++it) {
        const StateID state_id(it->second);
        const AbstractState s(it->first);
        if (deads[state_id]) {
            ++result.dead;
            if (store_deads) {
                result.dead_ends->set(s, true);
            }
        } else if (one && ones[state_id]) {
            ++result.one;
            result.one_states->set(s, true);
        } else {
            result.value_table->set(
                s,
                algorithms::interval_iteration::upper_bound(values[state_id]));
        }
    }

    return result;
}

AbstractAnalysisResult
compute_value_table(
    ProbabilisticProjection* projection,
    AnalysisObjective* objective,
    QualitativeResultStore* dead_ends)
{
    // filter states that can reach goal with absolute certainty
    bool separate_one_states =
        dynamic_cast<GoalProbabilityObjective*>(objective) != nullptr;
    if (dynamic_cast<GoalProbabilityObjective*>(objective) == nullptr) {
        g_err << "Probabilistic projections currently only support MaxGoalProb "
                 "objectives."
              << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    AbstractStateInStore is_goal(
        &projection->get_abstract_goal_states(),
        value_type::one,
        value_type::zero);
    AbstractTransitionNoReward no_reward;
    return projection->compute_value_table(
        &is_goal,
        &no_reward,
        value_type::zero,
        value_type::one,
        separate_one_states,
        value_type::one,
        dead_ends);
}

void
dump_graphviz(
    ProbabilisticProjection* projection,
    AnalysisObjective* objective,
    const std::string& path,
    bool transition_labels,
    bool values)
{
    if (dynamic_cast<GoalProbabilityObjective*>(objective) == nullptr) {
        g_err << "Probabilistic projections currently only support MaxGoalProb "
                 "objectives."
              << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    AbstractStateInStore is_goal(
        &projection->get_abstract_goal_states(),
        value_type::one,
        value_type::zero);
    if (values) {
        AbstractTransitionNoReward no_reward;
        AbstractAnalysisResult values = projection->compute_value_table(
            &is_goal,
            &no_reward,
            value_type::zero,
            value_type::one,
            true,
            value_type::one);
        projection->dump_graphviz(
            &is_goal,
            &values,
            value_type::zero,
            value_type::one,
            path,
            transition_labels);
        delete (values.value_table);
        delete (values.dead_ends);
        if (values.one_states != nullptr) {
            delete (values.one_states);
        }
    } else {
        projection->dump_graphviz(&is_goal, path, transition_labels);
    }
}

} // namespace pdbs
} // namespace probabilistic
