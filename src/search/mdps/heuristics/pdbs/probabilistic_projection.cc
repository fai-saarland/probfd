#include "probabilistic_projection.h"

#include "../../../global_operator.h"
#include "../../../globals.h"
#include "../../../successor_generator.h"
#include "../../../utils/hash.h"
#include "../../../utils/system.h"
#include "../../analysis_objective.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../engines/interval_iteration.h"
#include "../../globals.h"
#include "../../logging.h"
#include "../../probabilistic_operator.h"
#include "../../utils/graph_visualization.h"

#include <algorithm>
#include <deque>
#include <fstream>
#include <set>
#include <sstream>
#include <unordered_map>
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

    StateIDMap<AbstractState> state_id_map;

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    AbstractStateToString state_to_string(state_mapper_);
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        show_transition_labels ? &op_to_string : nullptr;

    std::ofstream out;
    out.open(path);

    graphviz::dump(
        out,
        initial_state_,
        &state_id_map,
        state_reward,
        &aops_gen,
        &transition_gen,
        &state_to_string,
        op_to_string_ptr,
        true);

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

    StateIDMap<AbstractState> state_id_map;

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

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
    AbstractOperatorToString* op_to_string_ptr =
        show_transition_labels ? &op_to_string : nullptr;

    std::ofstream out;
    out.open(path);

    graphviz::dump(
        out,
        initial_state_,
        &state_id_map,
        state_reward,
        &aops_gen,
        &transition_gen,
        &state_to_string,
        op_to_string_ptr,
        true);

    out.close();
}

AbstractAnalysisResult
ProbabilisticProjection::compute_value_table_expected_cost(
    AbstractStateEvaluator* state_reward,
    AbstractOperatorEvaluator* transition_reward,
    value_type::value_t dead_end_value,
    value_type::value_t upper)
{
    setup_abstract_operators();

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    topological_vi::TopologicalValueIteration
        <AbstractState, const AbstractOperator*, true>
        vi(nullptr,
           &state_id_map,
           &action_id_map,
           state_reward,
           transition_reward,
           dead_end_value,
           upper,
           &aops_gen,
           &transition_gen);

    topological_vi::ValueStore<std::false_type> value_table;
    vi.solve(initial_state_, value_table);

    AbstractAnalysisResult result;
    result.value_table = new QuantitativeResultStore();

    // FIXME Copying is expensive
    for (auto it = state_id_map.indirection_begin();
         it != state_id_map.indirection_end();
         ++it) {
        const StateID state_id(it->second);
        const AbstractState s(it->first);
        result.value_table->set(s, value_table[state_id].value);
    }

    result.reachable_states = state_id_map.size();

#if !defined(NDEBUG)
    {
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_variables().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_variables()[i];
        }
        logging::out << "]: value=" << (*result.value_table)[initial_state_]
                     << std::endl;
    }
#endif

    return result;
}

AbstractAnalysisResult
ProbabilisticProjection::compute_value_table(
    AbstractStateEvaluator* state_reward,
    AbstractOperatorEvaluator* transition_reward,
    value_type::value_t dead_end_value,
    value_type::value_t upper,
    bool one,
    value_type::value_t one_state_reward,
    QualitativeResultStore* dead_ends)
{
    setup_abstract_operators();

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    std::unique_ptr<AbstractStateEvaluator> heuristic = nullptr;
    if (dead_ends != nullptr) {
        heuristic = std::unique_ptr<AbstractStateEvaluator>(
            new AbstractStateInStoreEvaluator(
                dead_ends, value_type::zero, value_type::zero));
    }

    interval_iteration::
    IntervalIteration<AbstractState, const AbstractOperator*, true>
        vi(heuristic.get(),
           one,
           &state_id_map,
           &action_id_map,
           state_reward,
           transition_reward,
           dead_end_value,
           upper,
           &aops_gen,
           &transition_gen);

    interval_iteration::ValueStore values;
    // states that cannot reach goal
    interval_iteration::BoolStore deads(false);
    // states that can reach goal with absolute certainty
    interval_iteration::BoolStore ones(false);

    vi.solve(initial_state_, &values, &deads, &ones);

#if !defined(NDEBUG)
    {
        const StateID state_id = state_id_map.get_state_id(initial_state_);
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_variables().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_variables()[i];
        }
        logging::out << "]: lb="
                     << interval_iteration::lower_bound(values[state_id])
                     << ", ub="
                     << interval_iteration::upper_bound(values[state_id])
                     << ", error="
                     << (interval_iteration::upper_bound(values[state_id])
                         - interval_iteration::lower_bound(values[state_id]))
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
                s, interval_iteration::upper_bound(values[state_id]));
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
    if (dynamic_cast<GoalProbabilityObjective*>(objective)) {
        AbstractStateInStoreEvaluator is_goal(
            &projection->get_abstract_goal_states(),
            value_type::one,
            value_type::zero);
        ZeroCostActionEvaluator no_reward;

        return projection->compute_value_table(
            &is_goal,
            &no_reward,
            value_type::zero,
            value_type::one,
            true, // filter states that can reach goal with absolute certainty
            value_type::one,
            dead_ends);
    } else {
#if 0
        logging::err
            << "Probabilistic projections currently only support MaxGoalProb "
               "objectives."
            << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
#endif

        AbstractStateInStoreEvaluator is_goal(
            &projection->get_abstract_goal_states(),
            value_type::zero,
            value_type::zero);
        UnitCostActionEvaluator neg_unit_reward;

        return projection->compute_value_table_expected_cost(
            &is_goal,
            &neg_unit_reward,
            -value_type::inf,
            value_type::zero);
    }
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
        logging::err
            << "Probabilistic projections currently only support MaxGoalProb "
               "objectives."
            << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    AbstractStateInStoreEvaluator is_goal(
        &projection->get_abstract_goal_states(),
        value_type::one,
        value_type::zero);
    if (values) {
        ZeroCostActionEvaluator no_reward;
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
