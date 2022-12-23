#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "probfd/engines/interval_iteration.h"

#include "probfd/utils/graph_visualization.h"
#include "probfd/utils/logging.h"

#include "probfd/globals.h"

#include "pdbs/pattern_database.h"

#include "utils/collections.h"

#include "successor_generator.h"

#include "lp/lp_solver.h"

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
{
    compute_value_table(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

void MaxProbProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic)
{
    using namespace engine_interfaces;
    using namespace engines::interval_iteration;

    ZeroCostAbstractRewardFunction reward(
        goal_state_flags_,
        value_type::one,
        value_type::zero);

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        match_tree_);

    IntervalIteration<AbstractState, const AbstractOperator*> vi(
        &state_id_map,
        &action_id_map,
        &reward,
        &transition_gen,
        &heuristic,
        true,
        true);

    std::vector<StateID> proper_states;

    std::vector<value_utils::IntervalValue> interval_value_table(
        state_mapper_->num_states());

    vi.solve(initial_state_, interval_value_table, dead_ends_, proper_states);

    // We only need the upper bounds
    for (std::size_t i = 0; i != interval_value_table.size(); ++i) {
        value_table[i] = interval_value_table[i].upper;
    }

#if !defined(NDEBUG)
    logging::out << "(II) Pattern [";
    for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
        logging::out << (i > 0 ? ", " : "") << state_mapper_->get_pattern()[i];
    }

    logging::out << "]: value=" << interval_value_table[initial_state_.id]
                 << std::endl;

#if defined(USE_LP)
    verify(state_id_map);
#endif
#endif
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
    if (goal_state_flags_[initial_state_.id]) {
        return policy;
    }

    std::map<AbstractState, PredecessorList> predecessors;

    std::deque<AbstractState> open;
    std::unordered_set<AbstractState> closed;
    open.push_back(initial_state_);
    closed.insert(initial_state_);

    std::vector<AbstractState> goals;

    // Build the greedy policy graph
    while (!open.empty()) {
        AbstractState s = open.front();
        open.pop_front();

        // Skip dead-ends, the operator is irrelevant
        if (is_dead_end(s)) {
            continue;
        }

        const value_type::value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        match_tree_.get_applicable_operators(s, aops);

        // Select the greedy operators and add their successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = value_type::zero;

            std::vector<AbstractState> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                AbstractState t = s + eff;
                op_value += prob * value_table[t.id];
                successors.push_back(t);
            }

            if (value_type::approx_equal()(value, op_value)) {
                for (const AbstractState& succ : successors) {
                    if (goal_state_flags_[succ.id]) {
                        goals.push_back(succ);
                    } else if (!utils::contains(closed, succ)) {
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
    open.insert(open.end(), goals.begin(), goals.end());
    closed.clear();
    closed.insert(goals.begin(), goals.end());

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
                std::vector<const AbstractOperator*> aops;
                match_tree_.get_applicable_operators(pstate, aops);

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
    bool transition_labels)
{
    auto s2str = [this](const StateID& id, const AbstractState& x) {
        std::ostringstream out;
        out.precision(3);
        out << id.id;

            if (utils::contains(dead_ends_, StateID(x.id))) {
            out << "\\nh = 0 (dead)";
            } else {
            out << "\\nh = " << value_table[x.id];
        }

        return out.str();
    };

    ZeroCostAbstractRewardFunction reward(
        goal_state_flags_,
        value_type::one,
        value_type::zero);

    ProbabilisticProjection::dump_graphviz(
        path,
        s2str,
        reward,
        transition_labels);
}

#if !defined(NDEBUG) && defined(USE_LP)
void MaxProbProjection::verify(
    const engine_interfaces::StateIDMap<AbstractState>& state_id_map)
{
    lp::LPSolverType type;

#ifdef COIN_HAS_CLP
    type = lp::LPSolverType::CLP;
#elif defined(COIN_HAS_CPX)
    type = lp::LPSolverType::CPLEX;
#elif defined(COIN_HAS_GRB)
    type = lp::LPSolverType::GUROBI;
#elif defined(COIN_HAS_SPX)
    type = lp::LPSolverType::SOPLEX;
#else
    std::cerr << "Warning: Could not verify PDB value table since no LP solver"
        "is available !" << std::endl;
    return;
#endif

    lp::LPSolver solver(type);

    std::unordered_set<StateID> visited(
        state_id_map.visited_begin(),
        state_id_map.visited_end());

    std::vector<lp::LPVariable> variables;

    for (AbstractState s = AbstractState(0);
         s.id != static_cast<int>(state_mapper_->num_states());
         ++s.id) {
        variables.emplace_back(
            value_type::zero,
            value_type::one,
            value_type::one);
    }

    std::vector<lp::LPConstraint> constraints;

    std::deque<AbstractState> queue({initial_state_});
    std::set<AbstractState> seen({initial_state_});

    while (!queue.empty()) {
        AbstractState s = queue.front();
        queue.pop_front();

        assert(utils::contains(visited, StateID(s.id)));
        visited.erase(StateID(s.id));

        if (goal_state_flags_[s.id]) {
            auto& g =
                constraints.emplace_back(value_type::one, value_type::one);
            g.insert(s.id, value_type::one);
        }

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        match_tree_.get_applicable_operators(s, aops);

        // Select a greedy operators and add its successors
        for (const AbstractOperator* op : aops) {
            auto& constr =
                constraints.emplace_back(value_type::zero, value_type::inf);

            std::unordered_map<AbstractState, value_type::value_t>
                successor_dist;

            for (const auto& [eff, prob] : op->outcomes) {
                successor_dist[s + eff] -= prob;
            }

            if (successor_dist.size() == 1 &&
                successor_dist.begin()->first == s) {
                continue;
            }

            successor_dist[s] += value_type::one;

            for (const auto& [succ, prob] : successor_dist) {
                constr.insert(succ.id, prob);

                if (!utils::contains(seen, succ)) {
                    queue.push_back(succ);
                    seen.insert(succ);
                }
            }
        }
    }

    assert(visited.empty());

    solver.load_problem(lp::LPObjectiveSense::MINIMIZE, variables, constraints);

    solver.solve();

    assert(solver.has_optimal_solution());

    std::vector<double> solution = solver.extract_solution();

    for (AbstractState s(0); s.id != static_cast<int>(value_table.size());
         ++s.id) {
        if (utils::contains(seen, s)) {
            assert(value_type::approx_equal(
                0.001)(solution[s.id], value_table[s.id]));
        } else {
            assert(value_type::zero == value_table[s.id]);
        }
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
