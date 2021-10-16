#include "maxprob_projection.h"

#include "../../../global_operator.h"
#include "../../../successor_generator.h"
#include "../../analysis_objectives/goal_probability_objective.h"
#include "../../engines/interval_iteration.h"
#include "../../logging.h"
#include "../../utils/graph_visualization.h"

#include "../../../pdbs/pattern_database.h"

#include <deque>
#include <fstream>
#include <numeric>
#include <set>
#include <sstream>

namespace probabilistic {
namespace pdbs {

static std::vector<int> insert(std::vector<int> pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));
    auto it = std::lower_bound(pattern.begin(), pattern.end(), add_var);
    pattern.insert(it, add_var);
    return pattern;
}

namespace {
class WrapperHeuristic : public AbstractStateEvaluator {
    const QualitativeResultStore* dead_ends;
    const AbstractStateEvaluator& parent;

public:
    WrapperHeuristic(
        const QualitativeResultStore* dead_ends,
        const AbstractStateEvaluator& parent)
        : dead_ends(dead_ends)
        , parent(parent)
    {
    }

    virtual EvaluationResult evaluate(const AbstractState& state) const
    {
        if (dead_ends && dead_ends->operator[](state)) {
            return EvaluationResult{false, value_type::zero};
        }

        return parent(state);
    }
};
} // namespace

MaxProbProjection::MaxProbProjection(
    const Pattern& pattern,
    const std::vector<int>& domains,
    const AbstractStateEvaluator& heuristic,
    bool precompute_dead_ends)
    : ProbabilisticProjection(pattern, domains)
    , value_table(state_mapper_->size())
{
    initialize(heuristic, precompute_dead_ends);
}

MaxProbProjection::MaxProbProjection(
    const ::pdbs::PatternDatabase& pdb,
    bool precompute_dead_ends)
    : MaxProbProjection(
          pdb.get_pattern(),
          ::g_variable_domain,
          DeadendPDBEvaluator(pdb),
          precompute_dead_ends)
{
}

MaxProbProjection::MaxProbProjection(
    const MaxProbProjection& pdb,
    int add_var,
    bool precompute_dead_ends)
    : ProbabilisticProjection(
          insert(pdb.get_pattern(), add_var),
          ::g_variable_domain)
    , value_table(state_mapper_->size())
{
    initialize(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var),
        precompute_dead_ends);
}

void MaxProbProjection::initialize(
    const AbstractStateEvaluator& heuristic,
    bool precompute_dead_ends)
{
    if (precompute_dead_ends) {
        this->precompute_dead_ends();
        if (!is_dead_end(initial_state_)) {
            compute_value_table(WrapperHeuristic(&dead_ends, heuristic), false);
        }
    } else {
        compute_value_table(heuristic, true);
    }
}

void MaxProbProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic,
    bool store_dead_ends)
{
    using namespace engines::interval_iteration;

    AbstractStateInSetRewardFunction state_reward(
        &goal_states_,
        value_type::one,
        value_type::zero);
    ZeroCostActionEvaluator no_reward;

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map,
        state_mapper_,
        progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        state_mapper_,
        progression_aops_generator_);

    engines::interval_iteration::
        IntervalIteration<AbstractState, const AbstractOperator*, true>
            vi(&state_id_map,
               &action_id_map,
               &state_reward,
               &no_reward,
               value_type::zero,
               value_type::one,
               &aops_gen,
               &transition_gen,
               &heuristic,
               true);

    // states that cannot reach goal
    engines::interval_iteration::BoolStore deads(false);
    // states that can reach goal with absolute certainty
    engines::interval_iteration::BoolStore ones(false);

    vi.solve(initial_state_, value_table, deads, ones);

#if !defined(NDEBUG)
    {
        const StateID state_id = state_id_map.get_state_id(initial_state_);
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_pattern()[i];
        }

        const auto value = value_table[state_id];

        logging::out << "]: lb=" << value.lower << ", ub=" << value.upper
                     << ", error=" << value.error_bound() << std::endl;
    }
#endif

    reachable_states = state_id_map.size();

    for (const int id : state_id_map.visited()) {
        const AbstractState s(id);
        if (deads[s.id]) {
            ++n_dead_ends;
            if (!store_dead_ends) {
                dead_ends.set(s, true);
            }
        } else if (ones[s.id]) {
            ++n_one_states;
            one_states.set(s, true);
        }
    }

    all_one = num_one_states() == num_reachable_states();
    deterministic =
        num_one_states() + num_dead_ends() == num_reachable_states();

    if (is_all_one() || is_deterministic()) {
        value_table.clear();
        one_states.clear();
    }
}

unsigned int MaxProbProjection::num_dead_ends() const
{
    return n_dead_ends;
}

unsigned int MaxProbProjection::num_one_states() const
{
    return n_one_states;
}

bool MaxProbProjection::is_all_one() const
{
    return all_one;
}

bool MaxProbProjection::is_deterministic() const
{
    return deterministic;
}

bool MaxProbProjection::is_dead_end(const GlobalState& s) const
{
    return is_dead_end(get_abstract_state(s));
}

bool MaxProbProjection::is_dead_end(const AbstractState& s) const
{
    return !is_all_one() && dead_ends.get(s);
}

value_type::value_t MaxProbProjection::lookup(const GlobalState& s) const
{
    return lookup(get_abstract_state(s));
}

value_type::value_t MaxProbProjection::lookup(const AbstractState& s) const
{
    assert(!is_dead_end(s));
    return all_one || deterministic || one_states.get(s)
               ? value_type::one
               : value_table[s.id].upper;
}

EvaluationResult MaxProbProjection::evaluate(const GlobalState& s) const
{
    return evaluate(get_abstract_state(s));
}

EvaluationResult MaxProbProjection::evaluate(const AbstractState& s) const
{
    if (is_dead_end(s)) {
        return {true, value_type::zero};
    }

    const auto v = this->lookup(s);
    return {false, v};
}

namespace {
struct StateToString {
    explicit StateToString(
        bool all_one,
        bool deterministic,
        const std::vector<value_utils::IntervalValue>* value_table,
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
            out << value_table->operator[](x.id).upper;
        }
        out << "}";
        return out.str();
    }

    bool all_one;
    bool deterministic;
    const std::vector<value_utils::IntervalValue>* value_table;
    const QualitativeResultStore* one_states;
    const QualitativeResultStore* dead_ends;
    value_type::value_t v0;
    value_type::value_t v1;
    AbstractStateToString state_str;
};
} // namespace

void MaxProbProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values)
{
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        transition_labels ? &op_to_string : nullptr;

    if (values) {
        StateToString sts(
            all_one,
            deterministic,
            &value_table,
            &one_states,
            &dead_ends,
            value_type::zero,
            value_type::one,
            state_mapper_);

        ProbabilisticProjection::dump_graphviz(
            path,
            &sts,
            op_to_string_ptr,
            value_type::one);
    } else {
        AbstractStateToString sts(state_mapper_);
        ProbabilisticProjection::dump_graphviz(
            path,
            &sts,
            op_to_string_ptr,
            value_type::one);
    }
}

} // namespace pdbs
} // namespace probabilistic
