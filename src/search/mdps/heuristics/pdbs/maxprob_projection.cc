#include "maxprob_projection.h"

#include "../../../global_operator.h"
#include "../../../successor_generator.h"
#include "../../analysis_objective.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../engines/interval_iteration.h"
#include "../../globals.h"
#include "../../logging.h"
#include "../../utils/graph_visualization.h"

#include <deque>
#include <fstream>
#include <set>
#include <sstream>
#include <numeric>

namespace probabilistic {
namespace pdbs {

QualitativeResultStore
MaxProbProjection::compute_dead_ends()
{
    QualitativeResultStore result;
    if (projected_goal_.empty()) {
        result.negate_all();
        return result;
    }

    prepare_regression();

    // Initialize open list with goal states.
    std::deque<AbstractState> open;
    {
        AbstractState partial(0);
        std::vector<int> goal(state_mapper_->get_variables().size(), 0);
        std::vector<int> missing;
        int idx = 0;
        for (const auto& [var, val] : projected_goal_) {
            while (idx < var) {
                missing.push_back(idx++);
            }
            partial += state_mapper_->from_value_partial(var, val);
            goal[var] = val;
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

    // Start regression search.
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
            if (!result.get(t)) {
                result.set(t, true);
                open.push_back(t);
            }
        }
    }

    result.negate_all();
    return result;
}

void
MaxProbProjection::dump_graphviz(
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

    std::ofstream out(path);

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
}

void
MaxProbProjection::dump_graphviz(
    AbstractStateEvaluator* state_reward,
    MaxProbAbstractAnalysisResult* values,
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
            MaxProbAbstractAnalysisResult* values,
            value_type::value_t v0,
            value_type::value_t v1,
            std::shared_ptr<AbstractStateMapper> state_mapper)
            : values(values)
            , v0(v0)
            , v1(v1)
            , state_str(std::move(state_mapper))
        {
        }

        std::string operator()(const AbstractState& x) const
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

        MaxProbAbstractAnalysisResult* values;
        value_type::value_t v0;
        value_type::value_t v1;
        AbstractStateToString state_str;
    };

    StateToString state_to_string(values, v0, v1, state_mapper_);
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        show_transition_labels ? &op_to_string : nullptr;

    std::ofstream out(path);

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
}

MaxProbAbstractAnalysisResult
MaxProbProjection::compute_value_table(
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

    MaxProbAbstractAnalysisResult result;
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
         ++it)
    {
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

MaxProbAbstractAnalysisResult
compute_value_table(
    MaxProbProjection& projection,
    QualitativeResultStore* dead_ends)
{
        AbstractStateInStoreEvaluator is_goal(
            &projection.get_abstract_goal_states(),
            value_type::one,
            value_type::zero);
        ZeroCostActionEvaluator no_reward;

        return projection.compute_value_table(
            &is_goal,
            &no_reward,
            value_type::zero,
            value_type::one,
            true, // filter states that can reach goal with absolute certainty
            value_type::one,
            dead_ends);
}

void
dump_graphviz(
    MaxProbProjection& projection,
    const std::string& path,
    bool transition_labels,
    bool values)
{
    AbstractStateInStoreEvaluator is_goal(
        &projection.get_abstract_goal_states(),
        value_type::one,
        value_type::zero);

    if (values) {
        ZeroCostActionEvaluator no_reward;
        MaxProbAbstractAnalysisResult result = projection.compute_value_table(
            &is_goal,
            &no_reward,
            value_type::zero,
            value_type::one,
            true,
            value_type::one);
        projection.dump_graphviz(
            &is_goal,
            &result,
            value_type::zero,
            value_type::one,
            path,
            transition_labels);
        delete (result.value_table);
        delete (result.dead_ends);
        if (result.one_states != nullptr) {
            delete (result.one_states);
        }
    } else {
        projection.dump_graphviz(&is_goal, path, transition_labels);
    }
}

} // namespace pdbs
} // namespace probabilistic
