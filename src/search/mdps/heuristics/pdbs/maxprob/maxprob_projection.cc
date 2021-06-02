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
    const Pattern& pattern,
    const std::vector<int>& domains,
    bool precompute_dead_ends)
    : ProbabilisticProjection(pattern, domains)
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

void MaxProbProjection::prepare_regression()
{
    const Pattern& pattern = state_mapper_->get_pattern();

    std::vector<std::vector<std::pair<int, int>>> progressions;
    progressions.reserve(::g_operators.size());
    std::vector<AbstractState> operators;

    using footprint_t = std::pair<AbstractState, AbstractState>;
    utils::HashSet<footprint_t> operator_set;

    for (const auto& op : ::g_operators) {
        std::vector<std::pair<int, int>> projected_eff;
        std::vector<int> eff_no_pre;
        std::vector<int> precondition(pattern.size(), -1);
        {
            for (const auto [pre_var, pre_val] : op.get_preconditions()) {
                const int idx = var_index_[pre_var];
                if (idx != -1) {
                    precondition[idx] = pre_val;
                }
            }
        }

        AbstractState pre(0);
        AbstractState eff(0);
        {
            for (const auto [eff_var, eff_val, _] : op.get_effects()) {
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

        const auto add_operator = [&](AbstractState regression)
        {
            if (regression.id != 0 && operator_set.emplace(eff, regression).second) {
                progressions.push_back(projected_eff);
                operators.push_back(regression);
            }
        };

        state_mapper_->for_each_partial_state(pre - eff, eff_no_pre, add_operator);
    }

    regression_aops_generator_ =
        std::make_shared<RegressionSuccessorGenerator>(
            state_mapper_->get_domains(), progressions, operators);
}

void MaxProbProjection::precompute_dead_ends()
{
    prepare_regression();

    // Initialize open list with goal states.
    std::deque<AbstractState> open(goal_states_.begin(), goal_states_.end());

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
            if (!dead_ends.get(t)) {
                dead_ends.set(t, true);
                open.push_back(t);
            }
        }
    }

    dead_ends.negate_all();

    /*
     * TODO: Having the dead-ends, one could continue to compute the states
     * with MaxProb = 1. A simple fixed-point iteration achieves this:
     *
     * 1. Initialize S with non-dead-ends computed above.
     * 2. Regress from goal states and check for the states in S whether they
     *    are reached by any operator staying completely within S with all
     *    effects. If not, remove the state from from S.
     * 3. If S has changed go to 1.
     * 
     * In the end, S contains all states with MaxProb 1.
     */
}

void MaxProbProjection::compute_value_table(bool precomputed_dead_ends) {
    using namespace engines::interval_iteration;

    AbstractStateInSetEvaluator is_goal(
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
                     << lower_bound(values[state_id])
                     << ", ub="
                     << upper_bound(values[state_id])
                     << ", error="
                     << (upper_bound(values[state_id]) - lower_bound(values[state_id]))
                     << std::endl;
    }
#endif

    n_reachable_states = state_id_map.size();

    for (auto it = state_id_map.seen_begin();
         it != state_id_map.seen_begin();
         ++it)
    {
        const AbstractState s(*it);
        if (deads[s.id]) {
            ++n_dead_ends;
            if (!precomputed_dead_ends) {
                dead_ends.set(s, true);
            }
        } else if (ones[s.id]) {
            ++n_one_states;
            one_states.set(s, true);
        } else {
            value_table.set(s, upper_bound(values[s.id]));
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
    AbstractStateInSetEvaluator is_goal(
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
