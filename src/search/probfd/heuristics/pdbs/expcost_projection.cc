#include "probfd/heuristics/pdbs/expcost_projection.h"

#include "probfd/end_components/qualitative_reachability_analysis.h"

#include "probfd/engines/topological_value_iteration.h"

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
    const std::vector<StateID>& one_states;
    const AbstractStateEvaluator& parent;

public:
    WrapperHeuristic(
        const std::vector<StateID>& one_states,
        const AbstractStateEvaluator& parent)
        : one_states(one_states)
        , parent(parent)
    {
    }

    virtual EvaluationResult evaluate(const AbstractState& state) const
    {
        if (utils::contains(one_states, StateID(state.id))) {
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
    bool operator_pruning,
    const AbstractStateEvaluator& heuristic)
    : ExpCostProjection(
          new AbstractStateMapper(variables, domains),
          operator_pruning,
          heuristic)
{
}

ExpCostProjection::ExpCostProjection(
    AbstractStateMapper* mapper,
    bool operator_pruning,
    const AbstractStateEvaluator& heuristic)
    : ProbabilisticProjection(mapper, operator_pruning)
    , value_table(state_mapper_->num_states(), -value_type::inf)
{
    compute_value_table(heuristic);
}

ExpCostProjection::ExpCostProjection(
    const ::pdbs::PatternDatabase& pdb,
    bool operator_pruning)
    : ExpCostProjection(
          pdb.get_pattern(),
          ::g_variable_domain,
          operator_pruning,
          PDBEvaluator(pdb))
{
}

ExpCostProjection::ExpCostProjection(
    const ExpCostProjection& pdb,
    int add_var,
    bool operator_pruning)
    : ProbabilisticProjection(
          utils::insert(pdb.get_pattern(), add_var),
          ::g_variable_domain,
          operator_pruning)
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

AbstractPolicy ExpCostProjection::get_optimal_abstract_policy(
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    AbstractPolicy policy;

    assert(lookup(initial_state_) != -value_type::inf);

    if (utils::contains(goal_states_, initial_state_)) {
        return policy;
    }

    std::deque<AbstractState> open;
    std::unordered_set<AbstractState> closed;
    open.push_back(initial_state_);
    closed.insert(initial_state_);
    closed.insert(goal_states_.begin(), goal_states_.end());

    // Build the greedy policy graph
    while (!open.empty()) {
        AbstractState s = open.front();
        open.pop_front();

        const value_type::value_t value = value_table[s.id];

        // Generate operators...
        auto facts = state_mapper_->to_values(s);

        std::vector<const AbstractOperator*> aops;
        progression_aops_generator_->generate_applicable_ops(facts, aops);

        if (aops.empty()) {
            assert(value == -value_type::inf);
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng->shuffle(aops);

        const AbstractOperator* greedy_operator = nullptr;
        std::vector<AbstractState> greedy_successors;

        // Select first greedy operator
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = op->reward;

            std::vector<AbstractState> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                AbstractState t = s + eff;
                op_value += prob * value_table[t.id];
                successors.push_back(t);
            }

            if (value_type::approx_equal()(value, op_value)) {
                greedy_operator = op;
                greedy_successors = std::move(successors);
                break;
            }
        }

        assert(greedy_operator != nullptr);

        // Generate successors
        for (const AbstractState& succ : greedy_successors) {
            if (!utils::contains(closed, succ)) {
                closed.insert(succ);
                open.push_back(succ);
            }
        }

        // Collect all equivalent greedy operators
        std::vector<const AbstractOperator*> equivalent_operators;

        for (const AbstractOperator* op : aops) {
            if (op->outcomes.data() == greedy_operator->outcomes.data()) {
                equivalent_operators.push_back(op);
            }
        }

        // If wildcard consider all, else randomly pick one
        if (wildcard) {
            policy[s].insert(
                policy[s].end(),
                equivalent_operators.begin(),
                equivalent_operators.end());
        } else {
            policy[s].push_back(*rng->choose(equivalent_operators));
        }

        assert(!policy[s].empty());
    }

    return policy;
}

void ExpCostProjection::dump_graphviz(
    const std::string& path,
    bool transition_labels,
    bool values) const
{
    AbstractStateToString state_str(state_mapper_);

    auto s2str = [&, this](const StateID& id, const AbstractState& x) {
        std::ostringstream out;
        out << state_str(id, x);

        if (values) {
            out << " (" << value_table[x.id] << ")";
        }

        return out.str();
    };

    ProbabilisticProjection::dump_graphviz(
        path,
        s2str,
        transition_labels,
        value_type::zero);
}

void ExpCostProjection::compute_value_table(
    const AbstractStateEvaluator& heuristic)
{
    using namespace reachability;
    using namespace engine_interfaces;
    using namespace engines::topological_vi;

    NormalCostAbstractRewardFunction reward(
        &goal_states_,
        value_type::zero,
        value_type::zero);

    StateIDMap<AbstractState> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(abstract_operators_);

    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        state_mapper_,
        progression_aops_generator_);

    QualitativeReachabilityAnalysis<AbstractState, const AbstractOperator*>
        analysis(&state_id_map, &action_id_map, &reward, &transition_gen, true);

    std::vector<StateID> proper_states;

    analysis.run_analysis(
        initial_state_,
        std::back_inserter(dead_ends_),
        std::back_inserter(proper_states));

    WrapperHeuristic h(proper_states, heuristic);

    TopologicalValueIteration<AbstractState, const AbstractOperator*> vi(
        &state_id_map,
        &action_id_map,
        &reward,
        value_utils::IntervalValue(-value_type::inf, value_type::zero),
        &transition_gen,
        &h,
        true);

    vi.solve(state_id_map.get_state_id(initial_state_), value_table);

#if !defined(NDEBUG)
    logging::out << "(II) Pattern [";
    for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
        logging::out << (i > 0 ? ", " : "") << state_mapper_->get_pattern()[i];
    }

    logging::out << "]: value=" << value_table[initial_state_.id] << std::endl;

#if defined(USE_LP)
    verify(state_id_map, proper_states);
#endif
#endif
}

#if !defined(NDEBUG) && defined(USE_LP)
void ExpCostProjection::verify(
    const engine_interfaces::StateIDMap<AbstractState>& state_id_map,
    const std::vector<StateID>& proper_states)
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
                 "is available !"
              << std::endl;
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
        const bool in = utils::contains(proper_states, StateID(s.id));
        variables.emplace_back(
            -value_type::inf,
            value_type::zero,
            in ? value_type::one : value_type::zero);
    }

    std::vector<lp::LPConstraint> constraints;

    for (AbstractState s : goal_states_) {
        auto& g = constraints.emplace_back(value_type::zero, value_type::zero);
        g.insert(s.id, value_type::one);
    }

    std::deque<AbstractState> queue({initial_state_});
    std::set<AbstractState> seen({initial_state_});

    while (!queue.empty()) {
        AbstractState s = queue.front();
        queue.pop_front();

        assert(utils::contains(visited, StateID(s.id)));
        visited.erase(StateID(s.id));

        // Generate operators...
        auto facts = state_mapper_->to_values(s);

        std::vector<const AbstractOperator*> aops;
        progression_aops_generator_->generate_applicable_ops(facts, aops);

        // Select a greedy operators and add its successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t reward = op->reward;

            auto& constr = constraints.emplace_back(reward, value_type::inf);

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

    for (AbstractState s = AbstractState(0);
         s.id != static_cast<int>(state_mapper_->num_states());
         ++s.id) {
        if (utils::contains(proper_states, StateID(s.id)) &&
            utils::contains(seen, s)) {
            assert(value_type::approx_equal(
                0.001)(solution[s.id], value_table[s.id]));
        } else {
            assert(value_table[s.id] == -value_type::inf);
        }
    }
}
#endif

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
