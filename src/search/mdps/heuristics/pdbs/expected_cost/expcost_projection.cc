#include "expcost_projection.h"

#include "../../../../global_operator.h"
#include "../../../engines/interval_iteration.h"
#include "../../../logging.h"
#include "../../../globals.h"
#include "../../../utils/graph_visualization.h"

#include <deque>
#include <sstream>
#include <numeric>
#include <fstream>

namespace probabilistic {
namespace pdbs {

ExpCostProjection::ExpCostProjection(
    const std::vector<int> &variables,
    const std::vector<int> &domains)
    : ProbabilisticProjection(variables, domains)
{
    compute_value_table();
}

unsigned int ExpCostProjection::num_reachable_states() const {
    return reachable_states;
}

QuantitativeResultStore&
ExpCostProjection::get_value_table() {
    return value_table;
}

const QuantitativeResultStore&
ExpCostProjection::get_value_table() const {
    return value_table;
}

value_type::value_t
ExpCostProjection::lookup(const GlobalState& s) const
{
    return lookup(state_mapper_->operator()(s));
}

value_type::value_t
ExpCostProjection::lookup(const AbstractState& s) const
{
    return value_table.get(s);
}

void
ExpCostProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values) const
{
    if (values) {
        dump_graphviz_with_values(path, transition_labels);
    } else {
        dump_graphviz_no_values(path, transition_labels);
    }
}

void
ExpCostProjection::compute_value_table()
{
    AbstractStateInStoreEvaluator is_goal(
        &goal_states_,
        value_type::zero,
        value_type::zero);
    UnitCostActionEvaluator neg_unit_reward;

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
           &is_goal,
           &neg_unit_reward,
           -value_type::inf,
           value_type::zero,
           &aops_gen,
           &transition_gen);

    topological_vi::ValueStore<std::false_type> vi_value_table;
    vi.solve(initial_state_, vi_value_table);

    // FIXME Copying is expensive
    for (auto it = state_id_map.indirection_begin();
         it != state_id_map.indirection_end();
         ++it)
    {
        const StateID state_id(it->second);
        const AbstractState s(it->first);
        value_table.set(s, vi_value_table[state_id].value);
    }

    reachable_states = state_id_map.size();

#if !defined(NDEBUG)
    {
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_variables().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_variables()[i];
        }
        logging::out << "]: value=" << value_table[initial_state_]
                     << std::endl;
    }
#endif
}

void
ExpCostProjection::dump_graphviz_no_values(
    const std::string& path,
    bool show_transition_labels) const
{
    AbstractStateInStoreEvaluator is_goal(
        &goal_states_,
        value_type::zero,
        value_type::zero);

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
        &is_goal,
        &aops_gen,
        &transition_gen,
        &state_to_string,
        op_to_string_ptr,
        true);
}

void
ExpCostProjection::dump_graphviz_with_values(
    const std::string& path,
    bool show_transition_labels) const
{
    AbstractStateInStoreEvaluator is_goal(
        &goal_states_,
        value_type::zero,
        value_type::zero);

    setup_abstract_operators();

    StateIDMap<AbstractState> state_id_map;

    ApplicableActionsGenerator<const AbstractOperator*> aops_gen(
        state_id_map, state_mapper_, progression_aops_generator_);
    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map, state_mapper_, progression_aops_generator_);

    struct StateToString {
        explicit StateToString(
            const QuantitativeResultStore* value_table,
            std::shared_ptr<AbstractStateMapper> state_mapper)
            : value_table(value_table)
            , state_str(std::move(state_mapper))
        {
        }

        std::string operator()(const AbstractState& x) const
        {
            std::ostringstream out;
            out << state_str(x) << " {";
            out << value_table->get(x);
            out << "}";
            return out.str();
        }

        const QuantitativeResultStore* value_table;
        AbstractStateToString state_str;
    };

    StateToString state_to_string(&value_table, state_mapper_);
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        show_transition_labels ? &op_to_string : nullptr;

    std::ofstream out(path);

    graphviz::dump(
        out,
        initial_state_,
        &state_id_map,
        &is_goal,
        &aops_gen,
        &transition_gen,
        &state_to_string,
        op_to_string_ptr,
        true);
}

} // namespace pdbs
} // namespace probabilistic
