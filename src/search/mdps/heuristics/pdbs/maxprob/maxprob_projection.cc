#include "maxprob_projection.h"

#include "../../../../global_operator.h"
#include "../../../../successor_generator.h"
#include "../../../analysis_objective.h"
#include "../../../analysis_objectives/goal_probability_objective.h"
#include "../../../engines/interval_iteration.h"
#include "../../../globals.h"
#include "../../../logging.h"
#include "../../../utils/graph_visualization.h"

#include <deque>
#include <fstream>
#include <set>
#include <sstream>
#include <numeric>

namespace probabilistic {
namespace pdbs {

MaxProbProjection::
MaxProbProjection(
    const Pattern& variables,
    const std::vector<int>& domains,
    bool precompute_dead_ends)
    : ProbabilisticProjection(variables, domains)
{
    if (precompute_dead_ends) {
        this->precompute_dead_ends();
        if (!is_dead_end(initial_state_)) {
            compute_value_table(true);
        }
    } else {
        compute_value_table(false);
    }
}

void
MaxProbProjection::precompute_dead_ends()
{
    if (projected_goal_.empty()) {
        dead_ends.negate_all();
        return;
    }

    prepare_regression();

    // Initialize open list with goal states.
    std::deque<AbstractState> open;
    {
        AbstractState partial(0);
        std::vector<int> goal(state_mapper_->get_pattern().size(), 0);
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
        while (idx < static_cast<int>(state_mapper_->get_pattern().size())) {
            missing.push_back(idx++);
        }

        state_mapper_->enumerate(
            missing,
            goal,
            [this, &open, &partial](
                AbstractState add, const std::vector<int>&) {
                const AbstractState goal_state = partial + add;
                open.push_back(goal_state);
                dead_ends.set(goal_state, true);
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
            if (!dead_ends.get(t)) {
                dead_ends.set(t, true);
                open.push_back(t);
            }
        }
    }

    dead_ends.negate_all();
}

void
MaxProbProjection::compute_value_table(bool precomputed_dead_ends) {
    AbstractStateInStoreEvaluator is_goal(
        &goal_states_,
        value_type::one,
        value_type::zero);
    ZeroCostActionEvaluator no_reward;

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    std::unique_ptr<AbstractStateEvaluator> heuristic = nullptr;
    if (precomputed_dead_ends) {
        heuristic = std::unique_ptr<AbstractStateEvaluator>(
            new AbstractStateInStoreEvaluator(
                &dead_ends, value_type::zero, value_type::zero));
    }

    engines::interval_iteration::
    IntervalIteration<AbstractState, const AbstractOperator*, true>
        vi(heuristic.get(),
           true,
           &state_id_map,
           &action_id_map,
           &is_goal,
           &no_reward,
           value_type::zero,
           value_type::one,
           &aops_gen,
           &transition_gen);

    engines::interval_iteration::ValueStore values;
    // states that cannot reach goal
    engines::interval_iteration::BoolStore deads(false);
    // states that can reach goal with absolute certainty
    engines::interval_iteration::BoolStore ones(false);

    vi.solve(initial_state_, &values, &deads, &ones);

#if !defined(NDEBUG)
    {
        const StateID state_id = state_id_map.get_state_id(initial_state_);
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_pattern()[i];
        }
        logging::out << "]: lb="
                     << engines::interval_iteration::
                        lower_bound(values[state_id])
                     << ", ub="
                     << engines::interval_iteration::
                        upper_bound(values[state_id])
                     << ", error="
                     << (engines::interval_iteration::
                        upper_bound(values[state_id])
                        -
                        engines::interval_iteration::
                        lower_bound(values[state_id]))
                     << std::endl;
    }
#endif

    n_reachable_states = state_id_map.size();

    for (auto it = state_id_map.indirection_begin();
         it != state_id_map.indirection_end();
         ++it)
    {
        const StateID state_id(it->second);
        const AbstractState s(it->first);
        if (deads[state_id]) {
            ++n_dead_ends;
            if (!precomputed_dead_ends) {
                dead_ends.set(s, true);
            }
        } else if (ones[state_id]) {
            ++n_one_states;
            one_states.set(s, true);
        } else {
            value_table.set(
                s, engines::interval_iteration::upper_bound(values[state_id]));
        }
    }

    all_one = num_one_states() == num_reachable_states();
    deterministic = num_one_states() + num_dead_ends() == num_reachable_states();

    if (is_all_one() || is_deterministic()) {
        one_states.clear();
    }
}

AbstractState
MaxProbProjection::get_abstract_state(const GlobalState& s) const {
    return state_mapper_->operator()(s);
}

unsigned int
MaxProbProjection::num_reachable_states() const {
    return n_reachable_states;
}

unsigned int
MaxProbProjection::num_dead_ends() const {
    return n_dead_ends;
}

unsigned int
MaxProbProjection::num_one_states() const {
    return n_one_states;
}

bool
MaxProbProjection::is_all_one() const {
    return all_one;
}

bool
MaxProbProjection::is_deterministic() const {
    return deterministic;
}

bool
MaxProbProjection::is_dead_end(AbstractState s) const {
    return !is_all_one() && dead_ends.get(s);
}

bool
MaxProbProjection::is_dead_end(const GlobalState& s) const {
    return is_dead_end(get_abstract_state(s));
}

value_type::value_t
MaxProbProjection::lookup(AbstractState s) const {
    assert(!is_dead_end(s));
    return all_one || deterministic || one_states.get(s) ?
        value_type::one :
        value_table.get(s);
}

value_type::value_t
MaxProbProjection::lookup(const GlobalState& s) const {
    return lookup(get_abstract_state(s));
}

namespace {
struct StateToString {
    explicit StateToString(
        bool all_one,
        bool deterministic,
        const QuantitativeResultStore* value_table,
        const QualitativeResultStore* one_states,
        const QualitativeResultStore* dead_ends,
        value_type::value_t v0,
        value_type::value_t v1,
        std::shared_ptr<AbstractStateMapper> state_mapper)
        : all_one(all_one)
        , deterministic(deterministic)
        , value_table(value_table)
        , one_states(one_states)
        , dead_ends(dead_ends)
        , v0(v0)
        , v1(v1)
        , state_str(std::move(state_mapper))
    {
    }

    std::string operator()(const AbstractState& x) const
    {
        std::ostringstream out;
        out << state_str(x) << " {";
        if (!all_one && dead_ends->get(x)) {
            out << "dead:" << v0;
        } else if (all_one || deterministic || one_states->get(x)) {
            out << "one:" << v1;
        } else {
            out << value_table->get(x);
        }
        out << "}";
        return out.str();
    }

    bool all_one;
    bool deterministic;
    const QuantitativeResultStore* value_table;
    const QualitativeResultStore* one_states;
    const QualitativeResultStore* dead_ends;
    value_type::value_t v0;
    value_type::value_t v1;
    AbstractStateToString state_str;
};
}

void
MaxProbProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values)
{
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        transition_labels ? &op_to_string : nullptr;

    if (values) {
        StateToString state_to_string(
            all_one,
            deterministic,
            &value_table,
            &one_states,
            &dead_ends,
            value_type::zero,
            value_type::one,
            state_mapper_);

        dump_graphviz(path, &state_to_string, op_to_string_ptr);
    } else {
        AbstractStateToString state_to_string(state_mapper_);
        dump_graphviz(path, &state_to_string, op_to_string_ptr);
    }
}

template<typename StateToString, typename ActionToString>
void MaxProbProjection::dump_graphviz(
    const std::string &path,
    const StateToString *sts,
    const ActionToString *ats)
{
    AbstractStateInStoreEvaluator is_goal(
        &goal_states_,
        value_type::one,
        value_type::zero);

    StateIDMap<AbstractState> state_id_map;

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    std::ofstream out(path);

    graphviz::dump(
        out,
        initial_state_,
        &state_id_map,
        &is_goal,
        &aops_gen,
        &transition_gen,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace probabilistic
