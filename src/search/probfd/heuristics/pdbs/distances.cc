#include "probfd/heuristics/pdbs/distances.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"
#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/heuristics/pdbs/projection_state_space.h"

#if !defined(NDEBUG) && defined(USE_LP)
#include "downward/lp/lp_solver.h"

#include <deque>
#include <set>
#endif

namespace probfd::heuristics::pdbs {

#if !defined(NDEBUG) && defined(USE_LP)
static void verify(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    std::span<const value_t> value_table,
    const std::vector<StateID>& pruned_states)
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
    const value_t term_cost = mdp.get_non_goal_termination_cost();

    named_vector::NamedVector<lp::LPVariable> variables;

    const size_t num_states = value_table.size();

    for (size_t i = 0; i != num_states; ++i) {
        variables.emplace_back(0_vt, std::min(term_cost, inf), 0_vt);
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    std::deque<StateRank> queue({initial_state});
    std::set<StateRank> seen({initial_state});

    while (!queue.empty()) {
        StateRank s = queue.front();
        queue.pop_front();

        if (utils::contains(pruned_states, s)) {
            continue;
        }

        variables[s].objective_coefficient = 1_vt;

        if (mdp.is_goal(s)) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(s, 1_vt);
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        mdp.generate_applicable_actions(s, aops);

        // Push successors
        for (const ProjectionOperator* op : aops) {
            const value_t cost = mdp.get_action_cost(op);

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            if (successor_dist.is_dirac(s)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != s) {
                    non_loop_prob += prob;
                    constr.insert(succ.id, -prob);
                }

                if (seen.insert(mdp.get_state(succ)).second) {
                    queue.push_back(mdp.get_state(succ));
                }
            }

            constr.insert(s, non_loop_prob);
        }
    }

    solver.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(variables),
        std::move(constraints),
        inf));

    solver.solve();

    if (!solver.has_optimal_solution()) {
        if (solver.is_infeasible()) {
            std::cerr << "Critical error: LP was infeasible!" << std::endl;
        } else {
            assert(solver.is_unbounded());
            std::cerr << "Critical error: LP was unbounded!" << std::endl;
        }

        solver.print_failure_analysis();
        abort();
    }

    std::vector<double> solution = solver.extract_solution();

    for (StateRank s = 0; s != num_states; ++s) {
        if (utils::contains(pruned_states, s) || !seen.contains(s)) {
            assert(value_table[s] == term_cost);
        } else {
            assert(is_approx_equal(solution[s], value_table[s], 0.001));
        }
    }
}
#endif

void compute_value_table(
    ProjectionStateSpace& mdp,
    StateRank initial_state,
    const Evaluator<StateRank>& heuristic,
    std::span<value_t> value_table,
    double max_time)
{
    using namespace preprocessing;
    using namespace algorithms::ta_topological_vi;

    class WrapperHeuristic : public Evaluator<StateRank> {
        const std::vector<StateID>& pruned_states;
        const Evaluator<StateRank>& parent;
        const value_t pruned_value;

    public:
        WrapperHeuristic(
            const std::vector<StateID>& pruned_states,
            const Evaluator<StateRank>& parent,
            value_t pruned_value)
            : pruned_states(pruned_states)
            , parent(parent)
            , pruned_value(pruned_value)
        {
        }

        value_t evaluate(StateRank state) const override
        {
            if (utils::contains(pruned_states, state)) {
                return pruned_value;
            }

            return parent.evaluate(state);
        }
    };

    utils::CountdownTimer timer(max_time);

    QualitativeReachabilityAnalysis<StateRank, const ProjectionOperator*>
        analysis(true);

    std::vector<StateID> pruned_states;

    if (mdp.get_non_goal_termination_cost() == INFINITE_VALUE) {
        analysis.run_analysis(
            mdp,
            nullptr,
            initial_state,
            iterators::discarding_output_iterator{},
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    } else {
        analysis.run_analysis(
            mdp,
            nullptr,
            initial_state,
            std::back_inserter(pruned_states),
            iterators::discarding_output_iterator{},
            iterators::discarding_output_iterator{},
            timer.get_remaining_time());
    }

    WrapperHeuristic h(
        pruned_states,
        heuristic,
        mdp.get_non_goal_termination_cost());

    TATopologicalValueIteration<StateRank, const ProjectionOperator*> vi;
    vi.solve(mdp, h, initial_state, value_table, timer.get_remaining_time());

#if !defined(NDEBUG) && defined(USE_LP)
    verify(mdp, initial_state, value_table, pruned_states);
#endif
}

} // namespace probfd::heuristics::pdbs
