#include "probfd/merge_and_shrink/distances.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/distribution.h"
#include "probfd/mdp.h"
#include "probfd/transition_tail.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <deque>
#include <ranges>

#ifndef NDEBUG
#include <utility> // for std::cmp_equal
#endif

using namespace std;

namespace probfd::merge_and_shrink {

static void forward_search(
    const vector<vector<int>>& graph,
    std::vector<int>& queue,
    vector<bool>& liveness)
{
    while (!queue.empty()) {
        const int state = queue.back();
        queue.pop_back();

        for (int successor : graph[state]) {
            if (liveness[successor]) continue;
            liveness[successor] = true;
            queue.push_back(successor);
        }
    }
}

Distances::Distances(const TransitionSystem& transition_system)
    : transition_system(transition_system)
{
}

void Distances::compute_liveness()
{
    const int init_state = transition_system.get_init_state();

    if (goal_distances[init_state] == INFINITE_VALUE) {
        return;
    }

    vector<vector<int>> forward_graph(transition_system.get_size());
    for (const auto& local_label_info : transition_system.label_infos()) {
        for (const auto& [src, targets] : local_label_info.get_transitions()) {
            // Skip transitions which are not alive
            if (std::ranges::any_of(targets, [this](int state) {
                    return goal_distances[state] == INFINITE_VALUE;
                }))
                continue;

            for (int target : targets) {
                forward_graph[src].emplace_back(target);
            }
        }
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    std::vector<int> queue;
    liveness[init_state] = true;
    queue.push_back(init_state);
    forward_search(forward_graph, queue, liveness);

    liveness_computed = true;
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
            const value_t label_cost = label_info.get_cost();
            const auto& probabilities = label_info.get_probabilities();
            for (const auto& [src, targets] : label_info.get_transitions()) {
                auto& [_, successor_dist] =
                    transitions_[src].emplace_back(label_cost);
                for (auto [item, prob] : vws::zip(targets, probabilities)) {
                    successor_dist.add_probability(item, prob);
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
        int state,
        const ProbabilisticTransition* action,
        SuccessorDistribution& result) override
    {
        for (auto [succ, prob] : action->successor_dist) {
            if (succ == state) continue;
            result.add_non_source_probability(StateID(succ), prob);
        }
    }

    void generate_all_transitions(
        int state,
        std::vector<const ProbabilisticTransition*>& aops,
        std::vector<SuccessorDistribution>& successors) override
    {
        aops.reserve(transitions_[state].size());
        successors.reserve(transitions_[state].size());
        for (const ProbabilisticTransition& t : transitions_[state]) {
            aops.push_back(&t);
            auto& dist = successors.emplace_back();
            for (auto [succ, prob] : t.successor_dist) {
                if (succ == state) continue;
                dist.add_non_source_probability(StateID(succ), prob);
            }
        }
    }

    void generate_all_transitions(
        int state,
        std::vector<TransitionTailType>& transitions) override
    {
        transitions.reserve(transitions_[state].size());
        for (const ProbabilisticTransition& t : transitions_[state]) {
            auto& [action, successor_dist] = transitions.emplace_back(&t);
            for (auto [succ, prob] : t.successor_dist) {
                if (succ == state) continue;
                successor_dist.add_non_source_probability(StateID(succ), prob);
            }
        }
    }

    TerminationInfo get_termination_info(int state) override
    {
        return goal_flags_[state]
                   ? TerminationInfo::from_goal(0_vt)
                   : TerminationInfo::from_non_goal(INFINITE_VALUE);
    }

    value_t get_action_cost(const ProbabilisticTransition* action) override
    {
        return action->cost;
    }
};
} // namespace

void Distances::compute_goal_distances()
{
    using namespace algorithms::ta_topological_vi;

    assert(std::cmp_equal(goal_distances.size(), transition_system.get_size()));

    ExplicitMDP explicit_mdp(transition_system);

    TATopologicalValueIteration<int, const ProbabilisticTransition*> tatvi(
        0.001);

    for (int i = 0; i != transition_system.get_size(); ++i) {
        if (goal_distances[i] != -INFINITE_VALUE) continue; // Already seen
        tatvi.solve(
            explicit_mdp,
            heuristics::ConstantEvaluator<int>(0_vt),
            0,
            goal_distances);
    }

    goal_distances_computed = true;
}

void Distances::compute_distances(bool compute_liveness, utils::LogProxy& log)
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
        if (compute_liveness) {
            log << "liveness and ";
        }
        log << "goal distances";
    }

    goal_distances.resize(num_states, -INFINITE_VALUE);
    compute_goal_distances();

    if (compute_liveness) {
        liveness.resize(num_states, false);
        Distances::compute_liveness();
    }
}

void Distances::apply_abstraction(
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

    const int new_num_states = state_equivalence_relation.size();
    vector<bool> new_liveness;
    vector<value_t> new_goal_distances;
    if (compute_liveness) {
        new_liveness.resize(new_num_states, false);
    }

    new_goal_distances.resize(new_num_states, DISTANCE_UNKNOWN);

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const StateEquivalenceClass& state_eqv_class =
            state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto pos = state_eqv_class.begin();
        bool is_alive = false;

        if (compute_liveness) {
            is_alive = liveness[*pos];
        }

        const value_t new_goal_dist = goal_distances[*pos];

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
            compute_distances(compute_liveness, log);
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

void Distances::statistics(utils::LogProxy& log) const
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

} // namespace probfd::merge_and_shrink
