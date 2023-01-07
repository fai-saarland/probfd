#include "probfd/heuristics/pdbs/expcost_projection.h"

#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/end_components/qualitative_reachability_analysis.h"

#include "probfd/engines/topological_value_iteration.h"

#include "probfd/utils/graph_visualization.h"
#include "probfd/utils/logging.h"

#include "pdbs/pattern_database.h"

#include "utils/collections.h"

#include "lp/lp_solver.h"

#include <deque>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
class WrapperHeuristic : public StateRankEvaluator {
    const std::vector<StateID>& one_states;
    const StateRankEvaluator& parent;

public:
    WrapperHeuristic(
        const std::vector<StateID>& one_states,
        const StateRankEvaluator& parent)
        : one_states(one_states)
        , parent(parent)
    {
    }

    virtual EvaluationResult evaluate(const StateRank& state) const
    {
        if (utils::contains(one_states, StateID(state.id))) {
            return parent.evaluate(state);
        }

        return EvaluationResult{true, -value_type::inf};
    }
};
} // namespace

using namespace value_utils;

ExpCostProjection::ExpCostProjection(
    const ProbabilisticTaskProxy& task_proxy,
    const Pattern& variables,
    bool operator_pruning,
    const StateRankEvaluator& heuristic)
    : ExpCostProjection(
          task_proxy,
          new StateRankingFunction(task_proxy, variables),
          operator_pruning,
          heuristic)
{
}

ExpCostProjection::ExpCostProjection(
    const ProbabilisticTaskProxy& task_proxy,
    const ::pdbs::PatternDatabase& pdb,
    bool operator_pruning)
    : ExpCostProjection(
          task_proxy,
          pdb.get_pattern(),
          operator_pruning,
          PDBEvaluator(pdb))
{
}

ExpCostProjection::ExpCostProjection(
    const ProbabilisticTaskProxy& task_proxy,
    const ExpCostProjection& pdb,
    int add_var,
    bool operator_pruning)
    : ProbabilisticProjection(
          task_proxy,
          utils::insert(pdb.get_pattern(), add_var),
          operator_pruning,
          -value_type::inf)
{
    compute_value_table(
        IncrementalPPDBEvaluator(pdb, state_mapper_.get(), add_var));
}

ExpCostProjection::ExpCostProjection(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction* mapper,
    bool operator_pruning,
    const StateRankEvaluator& heuristic)
    : ProbabilisticProjection(
          task_proxy,
          mapper,
          operator_pruning,
          -value_type::inf)
{
    compute_value_table(heuristic);
}

EvaluationResult ExpCostProjection::evaluate(const State& s) const
{
    return evaluate(get_abstract_state(s));
}

EvaluationResult ExpCostProjection::evaluate(const StateRank& s) const
{
    const auto v = this->lookup(s);
    return {v == -value_type::inf, v};
}

AbstractPolicy ExpCostProjection::get_optimal_abstract_policy(
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard) const
{
    AbstractPolicy policy(state_mapper_->num_states());

    assert(lookup(abstract_state_space_.initial_state_) != -value_type::inf);

    if (abstract_state_space_
            .goal_state_flags_[abstract_state_space_.initial_state_.id]) {
        return policy;
    }

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(abstract_state_space_.initial_state_);
    closed.insert(abstract_state_space_.initial_state_);

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        const value_type::value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        abstract_state_space_.match_tree_.get_applicable_operators(s, aops);

        if (aops.empty()) {
            assert(value == -value_type::inf);
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng->shuffle(aops);

        const AbstractOperator* greedy_operator = nullptr;
        std::vector<StateRank> greedy_successors;

        // Select first greedy operator
        for (const AbstractOperator* op : aops) {
            value_type::value_t op_value = op->reward;

            std::vector<StateRank> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                StateRank t = s + eff;
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
        for (const StateRank& succ : greedy_successors) {
            if (!abstract_state_space_.goal_state_flags_[succ.id] &&
                !utils::contains(closed, succ)) {
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
    bool transition_labels) const
{
    auto s2str = [this](const StateID& id, const StateRank& x) {
        std::ostringstream out;
        out.precision(3);
        out << id.id;

        const auto v = value_table[x.id];
        if (v == -value_type::inf) {
            out << "\\nh = -&infin;";
        } else {
            out << "\\nh = " << v;
        }

        return out.str();
    };

    NormalCostAbstractRewardFunction reward(
        abstract_state_space_.goal_state_flags_,
        value_type::zero,
        -value_type::inf);

    ProbabilisticProjection::dump_graphviz(
        path,
        s2str,
        reward,
        transition_labels);
}

void ExpCostProjection::compute_value_table(const StateRankEvaluator& heuristic)
{
    using namespace reachability;
    using namespace engine_interfaces;
    using namespace engines::topological_vi;

    NormalCostAbstractRewardFunction reward(
        abstract_state_space_.goal_state_flags_,
        value_type::zero,
        -value_type::inf);

    StateIDMap<StateRank> state_id_map;
    ActionIDMap<const AbstractOperator*> action_id_map(
        abstract_state_space_.abstract_operators_);

    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        abstract_state_space_.match_tree_);

    QualitativeReachabilityAnalysis<StateRank, const AbstractOperator*>
        analysis(&state_id_map, &action_id_map, &reward, &transition_gen, true);

    std::vector<StateID> proper_states;

    analysis.run_analysis(
        abstract_state_space_.initial_state_,
        std::back_inserter(dead_ends_),
        std::back_inserter(proper_states));

    WrapperHeuristic h(proper_states, heuristic);

    TopologicalValueIteration<StateRank, const AbstractOperator*>
        vi(&state_id_map, &action_id_map, &reward, &transition_gen, &h, true);

    vi.solve(
        state_id_map.get_state_id(abstract_state_space_.initial_state_),
        value_table);

#if !defined(NDEBUG)
    logging::out << "(II) Pattern [";
    for (unsigned i = 0; i < state_mapper_->get_pattern().size(); ++i) {
        logging::out << (i > 0 ? ", " : "") << state_mapper_->get_pattern()[i];
    }

    logging::out << "]: value="
                 << value_table[abstract_state_space_.initial_state_.id]
                 << std::endl;

#if defined(USE_LP)
    verify(state_id_map, proper_states);
#endif
#endif
}

#if !defined(NDEBUG) && defined(USE_LP)
void ExpCostProjection::verify(
    const engine_interfaces::StateIDMap<StateRank>& state_id_map,
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
    const double inf = solver.get_infinity();

    std::unordered_set<StateID> visited(
        state_id_map.visited_begin(),
        state_id_map.visited_end());

    named_vector::NamedVector<lp::LPVariable> variables;

    for (StateRank s = StateRank(0);
         s.id != static_cast<int>(state_mapper_->num_states());
         ++s.id) {
        const bool in = utils::contains(proper_states, StateID(s.id));
        variables.emplace_back(
            -inf,
            value_type::zero,
            in ? value_type::one : value_type::zero);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({abstract_state_space_.initial_state_});
    std::set<StateRank> seen({abstract_state_space_.initial_state_});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        assert(utils::contains(visited, StateID(s.id)));
        visited.erase(StateID(s.id));

        if (abstract_state_space_.goal_state_flags_[s.id]) {
            auto& g =
                constraints.emplace_back(value_type::zero, value_type::zero);
            g.insert(s.id, value_type::one);
        }

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        abstract_state_space_.match_tree_.get_applicable_operators(s, aops);

        // Select a greedy operators and add its successors
        for (const AbstractOperator* op : aops) {
            value_type::value_t reward = op->reward;

            auto& constr = constraints.emplace_back(reward, inf);

            std::unordered_map<StateRank, value_type::value_t> successor_dist;

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

    solver.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MINIMIZE,
        std::move(variables),
        std::move(constraints),
        inf));

    solver.solve();

    assert(solver.has_optimal_solution());

    std::vector<double> solution = solver.extract_solution();

    for (StateRank s = StateRank(0);
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
