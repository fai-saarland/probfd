#include "maxprob_projection.h"

#include "../../../global_operator.h"
#include "../../../utils/collections.h"
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

namespace {
class WrapperHeuristic : public AbstractStateEvaluator {
    const std::unordered_map<AbstractState, int>& goal_distances;
    const AbstractStateEvaluator& parent;

public:
    WrapperHeuristic(
        const std::unordered_map<AbstractState, int>& goal_distances,
        const AbstractStateEvaluator& parent)
        : goal_distances(goal_distances)
        , parent(parent)
    {
    }

    virtual EvaluationResult evaluate(const AbstractState& state) const
    {
        auto it = goal_distances.find(state);

        if (it != goal_distances.end()) {
            return EvaluationResult{false, value_type::zero};
        }

        return parent(state);
    }
};
} // namespace

MaxProbProjection::MaxProbProjection(
    const Pattern& pattern,
    const std::vector<int>& domains,
    const AbstractStateEvaluator& heuristic)
    : ProbabilisticProjection(pattern, domains)
    , value_table(state_mapper_->size())
{

    compute_value_table(heuristic);
}

MaxProbProjection::MaxProbProjection(const ::pdbs::PatternDatabase& pdb)
    : MaxProbProjection(
          pdb.get_pattern(),
          ::g_variable_domain,
          DeadendPDBEvaluator(pdb))
{
}

MaxProbProjection::MaxProbProjection(const MaxProbProjection& pdb, int add_var)
    : ProbabilisticProjection(
          utils::insert(pdb.get_pattern(), add_var),
          ::g_variable_domain)
    , value_table(state_mapper_->size())
{
    compute_value_table(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

void MaxProbProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic)
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

    dead_ends.reset(new QualitativeResultStore());
    proper_states.reset(new QualitativeResultStore());

    vi.solve(initial_state_, value_table, *dead_ends, *proper_states);

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
}

value_type::value_t MaxProbProjection::lookup(const GlobalState& s) const
{
    return lookup(get_abstract_state(s));
}

value_type::value_t MaxProbProjection::lookup(const AbstractState& s) const
{
    assert(!is_dead_end(s));
    return value_table[s.id].upper;
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
        const std::vector<value_utils::IntervalValue>* value_table,
        const QualitativeResultStore* dead_ends,
        const QualitativeResultStore* one_states,
        std::shared_ptr<AbstractStateMapper> state_mapper)
        : value_table(value_table)
        , dead_ends(dead_ends)
        , one_states(one_states)
        , state_str(std::move(state_mapper))
    {
    }

    std::string operator()(const AbstractState& x) const
    {
        std::ostringstream out;
        out << state_str(x) << " {";
        if (dead_ends->get(x)) {
            out << "dead:" << value_type::zero;
        } else if (one_states->get(x)) {
            out << "one:" << value_type::one;
        } else {
            out << value_table->operator[](x.id).upper;
        }
        out << "}";
        return out.str();
    }

    const std::vector<value_utils::IntervalValue>* value_table;
    const QualitativeResultStore* dead_ends;
    const QualitativeResultStore* one_states;
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
            &value_table,
            dead_ends.get(),
            proper_states.get(),
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
