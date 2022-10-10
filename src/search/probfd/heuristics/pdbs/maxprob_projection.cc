#include "maxprob_projection.h"

#include "../../../pdbs/pattern_database.h"
#include "../../../successor_generator.h"
#include "../../../utils/collections.h"

#include "../../engines/interval_iteration.h"
#include "../../globals.h"
#include "../../logging.h"
#include "../../utils/graph_visualization.h"

#include <deque>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace probfd {
namespace heuristics {
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
    bool operator_pruning,
    const AbstractStateEvaluator& heuristic)
    : MaxProbProjection(
          new AbstractStateMapper(pattern, domains),
          operator_pruning,
          heuristic)
{
}

MaxProbProjection::MaxProbProjection(
    AbstractStateMapper* mapper,
    bool operator_pruning,
    const AbstractStateEvaluator& heuristic)
    : ProbabilisticProjection(mapper, operator_pruning)
    , value_table(state_mapper_->num_states())
{
    compute_value_table(heuristic);
}

MaxProbProjection::MaxProbProjection(
    const ::pdbs::PatternDatabase& pdb,
    bool operator_pruning)
    : MaxProbProjection(
          pdb.get_pattern(),
          ::g_variable_domain,
          operator_pruning,
          DeadendPDBEvaluator(pdb))
{
}

MaxProbProjection::MaxProbProjection(
    const MaxProbProjection& pdb,
    int add_var,
    bool operator_pruning)
    : ProbabilisticProjection(
          utils::insert(pdb.get_pattern(), add_var),
          ::g_variable_domain,
          operator_pruning)
    , value_table(state_mapper_->num_states())
{
    compute_value_table(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

void MaxProbProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic)
{
    using namespace engines::interval_iteration;

    ZeroCostAbstractRewardFunction reward(
        &goal_states_,
        value_type::one,
        value_type::zero);

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        state_mapper_,
        progression_aops_generator_);

    IntervalIteration<AbstractState, const AbstractOperator*> vi(
        &state_id_map,
        &action_id_map,
        &reward,
        value_utils::IntervalValue(value_type::zero, value_type::one),
        &transition_gen,
        &heuristic,
        true,
        true);

    std::vector<StateID> proper_states;

    vi.solve(initial_state_, value_table, dead_ends_, proper_states);

#if !defined(NDEBUG)
    logging::out << "(II) Pattern [";
    for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
        logging::out << (i > 0 ? ", " : "") << state_mapper_->get_pattern()[i];
    }

    logging::out << "]: value=" << value_table[initial_state_.id] << std::endl;

    verify(state_id_map);
#endif
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

AbstractPolicy MaxProbProjection::get_optimal_abstract_policy(
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    using PredecessorList =
        std::vector<std::pair<AbstractState, const AbstractOperator*>>;

    assert(!is_dead_end(initial_state_));

    AbstractPolicy policy;

    // return empty policy indicating unsolvable
    if (utils::contains(goal_states_, initial_state_)) {
        return policy;
    }

    std::map<AbstractState, PredecessorList> predecessors;

    std::deque<AbstractState> open;
    std::unordered_set<AbstractState> closed;
    open.push_back(initial_state_);
    closed.insert(initial_state_);
    closed.insert(goal_states_.begin(), goal_states_.end());

    // Build the greedy policy graph
    while (!open.empty()) {
        AbstractState s = open.front();
        open.pop_front();

        // Skip dead-ends, the operator is irrelevant
        if (is_dead_end(s)) {
            continue;
        }

        const value_type::value_t value = value_table[s.id].upper;

        // Generate operators...
        auto facts = state_mapper_->to_values(s);

        std::vector<const AbstractOperator*> aops;
        progression_aops_generator_->generate_applicable_ops(facts, aops);

        // Select the greedy operators and add their successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = value_type::zero;

            std::vector<AbstractState> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                AbstractState t = s + eff;
                op_value += prob * value_table[t.id].upper;
                successors.push_back(t);
            }

            if (value_type::approx_equal()(value, op_value)) {
                for (const AbstractState& succ : successors) {
                    if (!utils::contains(closed, succ)) {
                        closed.insert(succ);
                        open.push_back(succ);
                        predecessors[succ] = PredecessorList();
                    }

                    predecessors[succ].emplace_back(s, op);
                }
            }
        }
    }

    // Do regression search with duplicate checking through the constructed
    // graph, expanding predecessors randomly to select an optimal policy
    assert(open.empty());
    open.insert(open.end(), goal_states_.begin(), goal_states_.end());
    closed.clear();
    closed.insert(goal_states_.begin(), goal_states_.end());

    while (!open.empty()) {
        // Choose a random successor
        auto it = rng->choose(open);
        AbstractState s = *it;

        std::swap(*it, open.back());
        open.pop_back();

        // Consider predecessors in random order
        rng->shuffle(predecessors[s]);

        for (const auto& [pstate, sel_op] : predecessors[s]) {
            if (!utils::contains(closed, pstate)) {
                closed.insert(pstate);
                open.push_back(pstate);

                // Collect all equivalent greedy operators
                auto facts = state_mapper_->to_values(pstate);

                std::vector<const AbstractOperator*> aops;
                progression_aops_generator_->generate_applicable_ops(
                    facts,
                    aops);

                std::vector<const AbstractOperator*> equivalent_operators;

                for (const AbstractOperator* op : aops) {
                    if (op->outcomes.data() == sel_op->outcomes.data()) {
                        equivalent_operators.push_back(op);
                    }
                }

                // If wildcard consider all, else randomly pick one
                if (wildcard) {
                    policy[pstate].insert(
                        policy[pstate].end(),
                        equivalent_operators.begin(),
                        equivalent_operators.end());
                } else {
                    policy[pstate].push_back(
                        *rng->choose(equivalent_operators));
                }
            }
        }
    }

    return policy;
}

void MaxProbProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values)
{
    AbstractStateToString state_str(state_mapper_);

    auto s2str = [&, this](const StateID& id, const AbstractState& x) {
        std::ostringstream out;
        out << state_str(id, x);

        if (values) {
            out << " (";
            if (utils::contains(dead_ends_, StateID(x.id))) {
                out << "dead";
            } else if (utils::contains(goal_states_, x)) {
                out << "goal";
            } else {
                auto bounds = value_table[x.id];
                out << "[" << bounds.lower << ", " << bounds.upper << "]";
            }
            out << ")";
        }

        return out.str();
    };

    ProbabilisticProjection::dump_graphviz(
        path,
        s2str,
        transition_labels,
        value_type::one);
}

#ifndef NDEBUG
void MaxProbProjection::verify(const StateIDMap<AbstractState>& state_id_map)
{
    for (const int id : state_id_map.visited()) {
        AbstractState s(id);
        const value_type::value_t value = value_table[s.id].upper;

        if (utils::contains(goal_states_, s)) {
            assert(value == value_type::one);
            continue;
        }

        // Generate operators...
        auto facts = state_mapper_->to_values(s);

        std::vector<const AbstractOperator*> aops;
        progression_aops_generator_->generate_applicable_ops(facts, aops);

        if (aops.empty()) {
            assert(value == value_type::zero);
            continue;
        }

        // Select a greedy operators and add its successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = value_type::zero;

            std::vector<AbstractState> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                AbstractState t = s + eff;
                op_value += prob * value_table[t.id].upper;
                successors.push_back(t);
            }

            if (value_type::approx_less()(value, op_value)) {
                abort();
            }

            if (value_type::approx_equal()(value, op_value)) {
                goto continue_exploring;
            }
        }

        std::cerr << "Could not find greedy operator for abstract state " << s
                  << "!" << std::endl;
        abort();

    continue_exploring:;
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
