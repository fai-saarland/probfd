#include "probfd/merge_and_shrink/distances.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/distribution.h"
#include "probfd/mdp.h"
#include "probfd/transition.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/utils/logging.h"

#include "downward/lp/lp_solver.h"

#include <cassert>
#include <deque>
#include <ranges>

#ifndef NDEBUG
#include <utility> // for std::cmp_equal
#endif

using namespace std;

namespace probfd::merge_and_shrink {

void compute_liveness(
    const TransitionSystem& transition_system,
    std::span<const value_t> goal_distances,
    std::vector<bool>& liveness,
    std::vector<int> queue)
{
    int init_state = transition_system.get_init_state();

    auto is_unsolvable = [&](int state) {
        return goal_distances[state] == INFINITE_VALUE;
    };

    if (is_unsolvable(init_state)) return;

    vector<vector<int>> forward_graph(transition_system.get_size());
    for (const auto& local_label_info : transition_system.label_infos()) {
        for (const auto& [src, targets] : local_label_info.get_transitions()) {
            // Skip transitions which are not alive
            if (std::ranges::any_of(targets, is_unsolvable)) continue;

            for (int target : targets) {
                forward_graph[src].emplace_back(target);
            }
        }
    }

    liveness[init_state] = true;
    queue.push_back(init_state);

    while (!queue.empty()) {
        int state = queue.back();
        queue.pop_back();

        for (int successor : forward_graph[state]) {
            if (liveness[successor]) continue;
            liveness[successor] = true;
            queue.push_back(successor);
        }
    }
}

namespace {
struct ProbabilisticTransition {
    value_t cost;
    Distribution<int> successor_dist;
};

class ExplicitMDP : public MDP<int, const ProbabilisticTransition*> {
    std::vector<std::vector<ProbabilisticTransition>> transitions_;
    std::vector<bool> goal_flags_;

public:
    explicit ExplicitMDP(const TransitionSystem& transition_system)
        : transitions_(transition_system.get_size())
        , goal_flags_(transition_system.get_size())
    {
        namespace vws = std::views;

        // Set up transitions
        for (const auto& label_info : transition_system.label_infos()) {
            const value_t cost = label_info.get_cost();
            const auto& probabilities = label_info.get_probabilities();
            for (const auto& t : label_info.get_transitions()) {
                auto& tt = transitions_[t.src].emplace_back(cost);
                for (auto [item, prob] : vws::zip(t.targets, probabilities)) {
                    tt.successor_dist.add_probability(item, prob);
                }
            }
        }

        // Set up goal state flags
        for (int i = 0; i != transition_system.get_size(); ++i) {
            goal_flags_[i] = transition_system.is_goal_state(i);
        }
    }

    StateID get_state_id(int state) override { return StateID(state); }

    int get_state(StateID state_id) override
    {
        return static_cast<int>(state_id.id);
    }

    void generate_applicable_actions(
        int state,
        std::vector<const ProbabilisticTransition*>& result) override
    {
        result.reserve(transitions_[state].size());
        for (const ProbabilisticTransition& t : transitions_[state]) {
            result.push_back(&t);
        }
    }

    void generate_action_transitions(
        int,
        const ProbabilisticTransition* action,
        Distribution<StateID>& result) override
    {
        for (auto [item, prob] : action->successor_dist) {
            result.add_probability(StateID(item), prob);
        }
    }

    void generate_all_transitions(
        int state,
        std::vector<const ProbabilisticTransition*>& aops,
        std::vector<Distribution<StateID>>& successors) override
    {
        aops.reserve(transitions_[state].size());
        successors.reserve(transitions_[state].size());
        for (const ProbabilisticTransition& t : transitions_[state]) {
            aops.push_back(&t);
            auto& dist = successors.emplace_back();
            for (auto [item, prob] : t.successor_dist) {
                dist.add_probability(StateID(item), prob);
            }
        }
    }

    void generate_all_transitions(
        int state,
        std::vector<TransitionType>& transitions) override
    {
        transitions.reserve(transitions_[state].size());
        for (const ProbabilisticTransition& t : transitions_[state]) {
            auto& tr = transitions.emplace_back(&t);
            for (auto [item, prob] : t.successor_dist) {
                tr.successor_dist.add_probability(StateID(item), prob);
            }
        }
    }

    TerminationInfo get_termination_info(int state) override
    {
        return goal_flags_[state]
                   ? TerminationInfo::from_goal()
                   : TerminationInfo::from_non_goal(INFINITE_VALUE);
    }

    value_t get_action_cost(const ProbabilisticTransition* action) override
    {
        return action->cost;
    }
};
} // namespace

void Distances::compute_distances(
    const TransitionSystem& transition_system,
    bool do_compute_liveness,
    utils::LogProxy& log)
{
    /*
      This method computes the distances of abstract states to the abstract
      goal states ("abstract J*") and if specified, also computes the alive
      states.
    */

    if (log.is_at_least_verbose()) {
        log << transition_system.tag();
    }

    const int num_states = transition_system.get_size();

    if (num_states == 0) {
        if (log.is_at_least_verbose()) {
            log << "empty transition system, no distances to compute" << endl;
        }
        liveness_computed = true;
        goal_distances_computed = true;
        return;
    }

    if (log.is_at_least_verbose()) {
        log << "computing ";
        if (do_compute_liveness) {
            log << "liveness and ";
        }
        log << "goal distances";
    }

    goal_distances.resize(num_states);
    std::ranges::fill(goal_distances, -INFINITE_VALUE);
    compute_goal_distances(transition_system, goal_distances);
    goal_distances_computed = true;

    if (do_compute_liveness) {
        liveness.resize(num_states);
        std::ranges::fill(liveness, false);
        compute_liveness(transition_system, goal_distances, liveness);
        liveness_computed = true;
    }
}

void Distances::apply_abstraction(
    const TransitionSystem& transition_system,
    const StateEquivalenceRelation& state_equivalence_relation,
    bool compute_liveness,
    utils::LogProxy& log)
{
    assert(
        !compute_liveness ||
        (is_liveness_computed() &&
         state_equivalence_relation.size() < liveness.size()));
    assert(are_goal_distances_computed());
    assert(state_equivalence_relation.size() < goal_distances.size());

    int new_num_states = state_equivalence_relation.size();
    vector<bool> new_liveness;
    vector<value_t> new_goal_distances(new_num_states, DISTANCE_UNKNOWN);
    if (compute_liveness) {
        new_liveness.resize(new_num_states, false);
    }

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const StateEquivalenceClass& state_eqv_class =
            state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto pos = state_eqv_class.begin();
        bool is_alive = false;
        value_t new_goal_dist = -1;
        if (compute_liveness) {
            is_alive = liveness[*pos];
        }
        new_goal_dist = goal_distances[*pos];

        auto distance_different = [=, this](int state) {
            return (compute_liveness && liveness[state] != is_alive) ||
                   (goal_distances[state] != new_goal_dist);
        };

        if (std::any_of(++pos, state_eqv_class.end(), distance_different)) {
            // Not J*-preserving -> recompute
            if (log.is_at_least_verbose()) {
                log << transition_system.tag()
                    << "simplification was not f-preserving!" << endl;
            }
            liveness.clear();
            goal_distances.clear();
            liveness_computed = false;
            goal_distances_computed = false;
            compute_distances(transition_system, compute_liveness, log);
            return;
        }

        if (compute_liveness) {
            new_liveness[new_state] = is_alive;
        }

        new_goal_distances[new_state] = new_goal_dist;
    }

    liveness = std::move(new_liveness);
    goal_distances = std::move(new_goal_distances);
}

void Distances::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        if (is_liveness_computed()) {
            log << "Init distances: ";
            for (size_t i = 0; i < liveness.size(); ++i) {
                log << i << ": " << liveness[i];
                if (i != liveness.size() - 1) {
                    log << ", ";
                }
            }
            log << endl;
        }
        if (are_goal_distances_computed()) {
            log << "Goal distances: ";
            for (size_t i = 0; i < goal_distances.size(); ++i) {
                log << i << ": " << goal_distances[i] << ", ";
                if (i != goal_distances.size() - 1) {
                    log << ", ";
                }
            }
            log << endl;
        }
    }
}

void Distances::statistics(
    const TransitionSystem& transition_system,
    utils::LogProxy& log) const
{
    if (log.is_at_least_verbose()) {
        log << transition_system.tag();
        if (!are_goal_distances_computed()) {
            log << "goal distances not computed";
        } else if (transition_system.is_solvable(*this)) {
            log << "init h="
                << get_goal_distance(transition_system.get_init_state());
        } else {
            log << "transition system is unsolvable";
        }
        log << endl;
    }
}

static void verify(ExplicitMDP& mdp, std::span<const value_t> value_table)
{
    lp::LPSolverType type = lp::LPSolverType::CPLEX;

    lp::LPSolver solver(type);
    const double inf = solver.get_infinity();

    named_vector::NamedVector<lp::LPVariable> variables;
    std::vector<int> unsolvable_indices;

    const int num_states = static_cast<int>(value_table.size());

    for (int i = 0; i != num_states; ++i) {
        if (value_table[i] == INFINITE_VALUE) {
            unsolvable_indices.push_back(i);
            variables.emplace_back(0_vt, inf, 0_vt);
        } else {
            variables.emplace_back(0_vt, inf, 1_vt);
        }
    }

    named_vector::NamedVector<lp::LPConstraint> constraints;

    for (int i = 0; i != value_table.size(); ++i) {
        if (mdp.get_termination_info(i).is_goal_state()) {
            auto& g = constraints.emplace_back(0_vt, 0_vt);
            g.insert(i, 1_vt);
        }

        // Generate operators...
        std::vector<const ProbabilisticTransition*> aops;
        mdp.generate_applicable_actions(i, aops);

        // Push successors
        for (const ProbabilisticTransition* op : aops) {
            const value_t cost = mdp.get_action_cost(op);

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(i, op, successor_dist);

            if (successor_dist.is_dirac(i)) {
                continue;
            }

            auto& constr = constraints.emplace_back(-inf, cost);

            value_t non_loop_prob = 0_vt;
            for (const auto& [succ, prob] : successor_dist) {
                if (succ != static_cast<size_t>(i)) {
                    non_loop_prob += prob;
                    constr.insert(succ.id, -prob);
                }
            }

            constr.insert(i, non_loop_prob);
        }
    }

    auto variables_copy = variables;
    auto constraints_copy = constraints;

    solver.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(variables_copy),
        std::move(constraints_copy),
        inf));

    solver.solve();

    if (!solver.has_optimal_solution()) {
        if (solver.is_infeasible()) {
            std::cerr << "Critical error: LP was infeasible, so a negative "
                         "cost transition cycle exists!"
                      << std::endl;
        } else {
            assert(solver.is_unbounded());
            std::cerr << "Critical error: LP was unbounded, so an unsolvable "
                         "state has not been detected!"
                      << std::endl;
        }

        solver.print_failure_analysis();
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    std::vector<double> solution = solver.extract_solution();

    for (int i = 0; i != num_states; ++i) {
        if (value_table[i] != INFINITE_VALUE &&
            !is_approx_equal(solution[i], value_table[i], 0.001)) {
            std::cerr << "State value for state " << i << " was "
                      << value_table[i] << " but expected " << solution[i]
                      << "!" << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }

    if (unsolvable_indices.empty()) return;

    for (auto& variable : variables) {
        variable.objective_coefficient = 0_vt;
    }

    unsolvable_indices.push_back(unsolvable_indices.front());

    for (size_t i = 1; i < unsolvable_indices.size(); ++i) {
        int index = unsolvable_indices[i - 1];
        int next_index = unsolvable_indices[i];

        variables[index].objective_coefficient = 1_vt;

        auto& con = constraints.emplace_back(0_vt, 0_vt);
        con.insert(index, 1_vt);
        con.insert(next_index, -1_vt);
    }

    std::cout << "Checking unsolvability..." << std::endl;

    variables_copy = variables;
    constraints_copy = constraints;

    solver.load_problem(lp::LinearProgram(
        lp::LPObjectiveSense::MAXIMIZE,
        std::move(variables_copy),
        std::move(constraints_copy),
        inf));

    solver.solve();

    if (!solver.is_unbounded()) {
        std::cerr << "An unsolvable state is solvable!" << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
}

void compute_goal_distances(
    const TransitionSystem& transition_system,
    std::span<value_t> goal_distances)
{
    using namespace algorithms::ta_topological_vi;

    assert(std::cmp_equal(goal_distances.size(), transition_system.get_size()));

    ExplicitMDP explicit_mdp(transition_system);

    TATopologicalValueIteration<int, const ProbabilisticTransition*> tatvi(
        transition_system.get_size());

    for (int i = 0; i != transition_system.get_size(); ++i) {
        if (goal_distances[i] != -INFINITE_VALUE) continue; // Already seen
        tatvi.solve(
            explicit_mdp,
            heuristics::BlindEvaluator<int>(),
            i,
            goal_distances);
    }

    std::cout << "Verifying distances..." << std::endl;
    verify(explicit_mdp, goal_distances);
}

} // namespace probfd::merge_and_shrink
