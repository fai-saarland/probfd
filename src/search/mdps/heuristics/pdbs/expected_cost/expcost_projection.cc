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

ExpCostProjection::ExpCostProjection(const Pattern& variables)
    : ExpCostProjection(variables, ::g_variable_domain)
{
}

ExpCostProjection::
ExpCostProjection(const Pattern& variables, const std::vector<int> &domains)
    : ProbabilisticProjection(variables, domains)
{
    compute_value_table();
}

value_type::value_t
ExpCostProjection::get_value(const GlobalState &state) const {
    auto abstract_state = state_mapper_->operator()(state);
    std::optional v = value_table.get_optional(abstract_state);
    return v ? *v : std::numeric_limits<value_type::value_t>::infinity();
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

namespace {
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
        out << state_str(x) << " {" << value_table->get(x) << "}";
        return out.str();
    }

    const QuantitativeResultStore* value_table;
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
ExpCostProjection::compute_value_table()
{
    AbstractStateInStoreEvaluator is_goal(
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
        vi(nullptr,
           &state_id_map,
           &action_id_map,
           &is_goal,
           &action_eval,
           -value_type::inf,
           value_type::zero,
           &aops_gen,
           &transition_gen);

    engines::topological_vi::ValueStore<std::false_type> vi_value_table;
    vi.solve(initial_state_, vi_value_table);

    // FIXME Copying is expensive
    for (size_t i = 0; i != state_mapper_->size(); ++i)
    {
        value_table.set(AbstractState(i), vi_value_table[i]);
    }

    reachable_states = state_id_map.size();

#if !defined(NDEBUG)
    {
        logging::out << "(II) Pattern [";
        for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
            logging::out << (i > 0 ? ", " : "")
                         << state_mapper_->get_pattern()[i];
        }
        logging::out << "]: value=" << value_table[initial_state_]
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
    AbstractStateInStoreEvaluator is_goal(
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
        &is_goal,
        &aops_gen,
        &transition_gen,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace probabilistic
