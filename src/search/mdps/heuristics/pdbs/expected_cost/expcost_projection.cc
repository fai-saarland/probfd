#include "expcost_projection.h"

#include "../../../../global_operator.h"
#include "../../../engines/interval_iteration.h"
#include "../../../logging.h"

#include <deque>
#include <sstream>
#include <numeric>

namespace probabilistic {
namespace pdbs {

ExpCostAbstractAnalysisResult
ExpCostProjection::compute_value_table(
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

    ExpCostAbstractAnalysisResult result;
    result.value_table = new QuantitativeResultStore();

    // FIXME Copying is expensive
    for (auto it = state_id_map.indirection_begin();
         it != state_id_map.indirection_end();
         ++it)
    {
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

ExpCostAbstractAnalysisResult
compute_value_table(ExpCostProjection& projection) {
    AbstractStateInStoreEvaluator is_goal(
        &projection.get_abstract_goal_states(),
        value_type::zero,
        value_type::zero);
    UnitCostActionEvaluator neg_unit_reward;

    return projection.compute_value_table(
        &is_goal,
        &neg_unit_reward,
        -value_type::inf,
        value_type::zero);
}

} // namespace pdbs
} // namespace probabilistic
