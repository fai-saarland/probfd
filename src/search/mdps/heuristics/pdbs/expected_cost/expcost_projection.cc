#include "expcost_projection.h"

#include "../../../../global_operator.h"
#include "../../../engines/interval_iteration.h"
#include "../../../logging.h"
#include "../../../globals.h"
#include "../../../utils/graph_visualization.h"

#include "../../../../pdbs/pattern_database.h"

#include <deque>
#include <sstream>
#include <numeric>
#include <fstream>

namespace probabilistic {
namespace pdbs {

using namespace value_utils;

ExpCostProjection::
ExpCostProjection(const Pattern& variables, AbstractStateEvaluator* heuristic)
    : ExpCostProjection(variables, ::g_variable_domain, heuristic)
{
}

ExpCostProjection::
ExpCostProjection(
    const Pattern& variables,
    const std::vector<int> &domains,
    AbstractStateEvaluator* heuristic)
    : ProbabilisticProjection(variables, domains)
    , value_table(
        state_mapper_->size(),
        SingleValue(
            -std::numeric_limits<value_type::value_t>::infinity()))
{
    compute_value_table(heuristic);
}

ExpCostProjection::ExpCostProjection(const ::pdbs::PatternDatabase& pdb)
    : ProbabilisticProjection(pdb.get_pattern(), ::g_variable_domain)
    , value_table(
        state_mapper_->size(),
        SingleValue(
            -std::numeric_limits<value_type::value_t>::infinity()))
{
    PDBEvaluator heuristic(pdb);
    compute_value_table(&heuristic);
}

value_type::value_t
ExpCostProjection::get_value(const GlobalState &state) const {
    auto abstract_state = state_mapper_->operator()(state);
    return lookup(abstract_state);
}

unsigned int ExpCostProjection::num_reachable_states() const {
    return reachable_states;
}

value_type::value_t
ExpCostProjection::lookup(const GlobalState& s) const
{
    return lookup(state_mapper_->operator()(s));
}

value_type::value_t
ExpCostProjection::lookup(const AbstractState& s) const
{
    return value_table[s.id];
}

namespace {
struct StateToString {
    explicit StateToString(
        const std::vector<SingleValue>* value_table,
        std::shared_ptr<AbstractStateMapper> state_mapper)
        : value_table(value_table)
        , state_str(std::move(state_mapper))
    {
    }

    std::string operator()(const AbstractState& x) const
    {
        std::ostringstream out;
        out << state_str(x) << " {"
            << as_upper_bound((*value_table)[x.id]) << "}";
        return out.str();
    }

    const std::vector<SingleValue>* value_table;
    AbstractStateToString state_str;
};
}

void
ExpCostProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values) const
{
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        transition_labels ? &op_to_string : nullptr;

    if (values) {
        StateToString sts(&value_table, state_mapper_);
        dump_graphviz(path, &sts, op_to_string_ptr);
    } else {
        AbstractStateToString sts(state_mapper_);
        dump_graphviz(path, &sts, op_to_string_ptr);
    }
}

void
ExpCostProjection::compute_value_table(AbstractStateEvaluator* heuristic)
{
    assert(heuristic);

    AbstractStateInSetRewardFunction state_reward(
        &goal_states_,
        value_type::zero,
        value_type::zero);
    NormalCostActionEvaluator action_eval;

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    engines::topological_vi::TopologicalValueIteration
        <AbstractState, const AbstractOperator*, true>
        vi(&state_id_map,
           &action_id_map,
           &state_reward,
           &action_eval,
           -value_type::inf,
           value_type::zero,
           &aops_gen,
           &transition_gen,
           SingleValue(value_type::zero),
           heuristic);

    vi.solve(initial_state_, value_table);

    reachable_states = state_id_map.size();

#if !defined(NDEBUG)
    {
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_pattern()[i];
        }
        logging::out << "]: value="
                     << as_upper_bound(value_table[initial_state_.id])
                     << std::endl;
    }
#endif
}

template<typename StateToString, typename ActionToString>
void
ExpCostProjection::dump_graphviz(
    const std::string& path,
    const StateToString* sts,
    const ActionToString* ats) const
{
    AbstractStateInSetRewardFunction state_reward(
        &goal_states_,
        value_type::zero,
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
        &state_reward,
        &aops_gen,
        &transition_gen,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace probabilistic
