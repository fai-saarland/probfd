#include "expcost_projection.h"

#include "../../../pdbs/pattern_database.h"
#include "../../../successor_generator.h"
#include "../../../utils/collections.h"

#include "../../engines/topological_value_iteration.h"
#include "../../globals.h"
#include "../../logging.h"
#include "../../utils/graph_visualization.h"

#include <deque>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace probabilistic {
namespace pdbs {

namespace {
class WrapperHeuristic : public AbstractStateEvaluator {
    const QualitativeResultStore& one_states;
    const AbstractStateEvaluator& parent;

public:
    WrapperHeuristic(
        const QualitativeResultStore& one_states,
        const AbstractStateEvaluator& parent)
        : one_states(one_states)
        , parent(parent)
    {
    }

    virtual EvaluationResult evaluate(const AbstractState& state) const
    {
        if (one_states[state]) {
            return parent(state);
        }

        return EvaluationResult{true, -value_type::inf};
    }
};
} // namespace

using namespace value_utils;

ExpCostProjection::ExpCostProjection(
    const Pattern& variables,
    const std::vector<int>& domains,
    const AbstractStateEvaluator& heuristic)
    : ExpCostProjection(new AbstractStateMapper(variables, domains), heuristic)
{
}

ExpCostProjection::ExpCostProjection(
    AbstractStateMapper* mapper,
    const AbstractStateEvaluator& heuristic)
    : ProbabilisticProjection(mapper)
    , value_table(state_mapper_->num_states(), -value_type::inf)
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
          utils::insert(pdb.get_pattern(), add_var),
          ::g_variable_domain)
    , value_table(state_mapper_->num_states(), -value_type::inf)
{
    compute_value_table(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

value_type::value_t ExpCostProjection::lookup(const GlobalState& s) const
{
    return lookup(get_abstract_state(s));
}

value_type::value_t ExpCostProjection::lookup(const AbstractState& s) const
{
    return value_table[s.id];
}

EvaluationResult ExpCostProjection::evaluate(const GlobalState& s) const
{
    return evaluate(get_abstract_state(s));
}

EvaluationResult ExpCostProjection::evaluate(const AbstractState& s) const
{
    const auto v = this->lookup(s);
    return {v == -value_type::inf, v};
}

AbstractPolicy ExpCostProjection::get_optimal_abstract_policy() const
{
    AbstractPolicy policy;

    // return empty policy indicating unsolvable
    if (lookup(initial_state_) == -value_type::inf) {
        return policy;
    }

    std::deque<AbstractState> open;
    std::unordered_set<AbstractState> closed;
    open.push_back(initial_state_);
    closed.insert(initial_state_);

    // Build the greedy policy graph
    while (!open.empty()) {
        AbstractState s = open.front();
        open.pop_front();

        const value_type::value_t value = value_table[s.id];

        // Generate operators...
        auto facts = state_mapper_->to_values(s);

        std::vector<const AbstractOperator*> aops;
        progression_aops_generator_->generate_applicable_ops(facts, aops);

        // Select a greedy operators and add its successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = value_type::zero;

            std::vector<AbstractState> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                AbstractState t = s + eff;
                op_value += prob * value_table[t.id];
                successors.push_back(t);
            }

            if (value_type::approx_equal()(value, op_value)) {
                policy[s] = op;

                for (const AbstractState& succ : successors) {
                    if (!utils::contains(closed, succ)) {
                        closed.insert(succ);
                        open.push_back(succ);
                    }
                }

                goto continue_exploring;
            }
        }

        continue_exploring:;
    }

    return policy;
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
    compute_proper_states();

    WrapperHeuristic h(*proper_states, heuristic);

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
        TopologicalValueIteration<AbstractState, const AbstractOperator*>
            vi(&state_id_map,
               &action_id_map,
               &state_reward,
               &action_eval,
               -value_type::inf,
               value_type::zero,
               &aops_gen,
               &transition_gen,
               &h,
               true);

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

} // namespace pdbs
} // namespace probabilistic
