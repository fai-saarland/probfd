#include "expcost_projection.h"

#include "../../../../global_operator.h"
#include "../../../engines/interval_iteration.h"
#include "../../../globals.h"
#include "../../../logging.h"
#include "../../../utils/graph_visualization.h"

#include "../../../../pdbs/pattern_database.h"

#include <deque>
#include <fstream>
#include <numeric>
#include <sstream>

namespace probabilistic {
namespace pdbs {
namespace expected_cost {

using namespace value_utils;

static std::vector<int> insert(std::vector<int> pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));
    auto it = std::lower_bound(pattern.begin(), pattern.end(), add_var);
    pattern.insert(it, add_var);
    return pattern;
}

ExpCostProjection::ExpCostProjection(
    const Pattern& variables,
    const std::vector<int>& domains,
    const AbstractStateEvaluator& heuristic)
    : ProbabilisticProjection(variables, domains)
    , value_table(state_mapper_->size(), -value_type::inf)
{
    compute_value_table(heuristic);
}

ExpCostProjection::ExpCostProjection(const ::pdbs::PatternDatabase& pdb)
    : ExpCostProjection(
          pdb.get_pattern(),
          ::g_variable_domain,
          PDBEvaluator(pdb))
{
}

ExpCostProjection::ExpCostProjection(const ExpCostProjection& pdb, int add_var)
    : ProbabilisticProjection(
          insert(pdb.get_pattern(), add_var),
          ::g_variable_domain)
    , value_table(state_mapper_->size(), -value_type::inf)
{
    compute_value_table(ExpCostPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

value_type::value_t ExpCostProjection::lookup(const GlobalState& s) const
{
    return lookup(state_mapper_->operator()(s));
}

value_type::value_t ExpCostProjection::lookup(const AbstractState& s) const
{
    return value_table[s.id];
}

namespace {
struct StateToString {
    explicit StateToString(
        const std::vector<value_type::value_t>* value_table,
        std::shared_ptr<AbstractStateMapper> state_mapper)
        : value_table(value_table)
        , state_str(std::move(state_mapper))
    {
    }

    std::string operator()(const AbstractState& x) const
    {
        std::ostringstream out;
        out << state_str(x) << " {" << as_upper_bound((*value_table)[x.id])
            << "}";
        return out.str();
    }

    const std::vector<value_type::value_t>* value_table;
    AbstractStateToString state_str;
};
} // namespace

void ExpCostProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values) const
{
    AbstractOperatorToString op_to_string(&g_operators);
    AbstractOperatorToString* op_to_string_ptr =
        transition_labels ? &op_to_string : nullptr;

    if (values) {
        StateToString sts(&value_table, state_mapper_);
        ProbabilisticProjection::dump_graphviz(
            path,
            &sts,
            op_to_string_ptr,
            value_type::zero);
    } else {
        AbstractStateToString sts(state_mapper_);
        ProbabilisticProjection::dump_graphviz(
            path,
            &sts,
            op_to_string_ptr,
            value_type::zero);
    }
}

void ExpCostProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic)
{
    AbstractStateInSetRewardFunction state_reward(
        &goal_states_,
        value_type::zero,
        value_type::zero);
    NormalCostActionEvaluator action_eval;

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

    engines::topological_vi::
        TopologicalValueIteration<AbstractState, const AbstractOperator*, true>
            vi(&state_id_map,
               &action_id_map,
               &state_reward,
               &action_eval,
               -value_type::inf,
               value_type::zero,
               &aops_gen,
               &transition_gen,
               &heuristic);

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

} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic
