#include "probfd/merge_and_shrink/distances.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <deque>

using namespace std;

namespace probfd::merge_and_shrink {

static void dijkstra_search(
    const vector<vector<pair<int, value_t>>>& graph,
    priority_queues::HeapQueue<value_t, int>& queue,
    vector<value_t>& distances)
{
    while (!queue.empty()) {
        const auto [distance, state] = queue.pop();

        const value_t state_distance = distances[state];

        assert(state_distance <= distance);

        if (state_distance < distance) continue;

        for (size_t i = 0; i < graph[state].size(); ++i) {
            const auto [successor, cost] = graph[state][i];
            if (const value_t successor_cost = state_distance + cost;
                distances[successor] > successor_cost) {
                distances[successor] = successor_cost;
                queue.push(successor_cost, successor);
            }
        }
    }
}

Distances::Distances(const TransitionSystem& transition_system)
    : transition_system(transition_system)
{
}

void Distances::compute_init_distances()
{
    vector<vector<pair<int, value_t>>> forward_graph(
        transition_system.get_size());
    for (const LocalLabelInfo& local_label_info : transition_system) {
        const value_t cost = local_label_info.get_cost();
        for (const auto& [src, targets] : local_label_info.get_transitions()) {
            for (int target : targets) {
                forward_graph[src].emplace_back(target, cost);
            }
        }
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    priority_queues::HeapQueue<value_t, int> queue;
    init_distances[transition_system.get_init_state()] = 0;
    queue.push(0, transition_system.get_init_state());
    dijkstra_search(forward_graph, queue, init_distances);

    init_distances_computed = true;
}

void Distances::compute_goal_distances()
{
    // TODO

    goal_distances_computed = true;
}

void Distances::compute_distances(
    bool compute_init_distances,
    bool compute_goal_distances,
    utils::LogProxy& log)
{
    assert(compute_init_distances || compute_goal_distances);
    /*
      This method does the following:
      - Computes the distances of abstract states from the abstract
        initial state ("abstract g") and to the abstract goal states
        ("abstract h"), depending on the given flags.
    */

#ifndef NDEBUG
    if (are_init_distances_computed()) {
        /*
          The only scenario where distance information is allowed to be
          present when computing distances is when computing goal distances
          for the final transition system in a setting where only init
          distances have been computed during the merge-and-shrink computation.
        */
        assert(!are_goal_distances_computed());
        assert(goal_distances.empty());
        assert(!compute_init_distances);
        assert(compute_goal_distances);
    } else {
        /*
          Otherwise, when computing distances, the previous (invalid)
          distance information must have been cleared before.
        */
        assert(
            !are_init_distances_computed() && !are_goal_distances_computed());
        assert(init_distances.empty() && goal_distances.empty());
    }
#endif

    if (log.is_at_least_verbose()) {
        log << transition_system.tag();
    }

    const int num_states = transition_system.get_size();

    if (num_states == 0) {
        if (log.is_at_least_verbose()) {
            log << "empty transition system, no distances to compute" << endl;
        }
        init_distances_computed = true;
        goal_distances_computed = true;
        return;
    }

    if (log.is_at_least_verbose()) {
        log << "computing ";
        if (compute_init_distances && compute_goal_distances) {
            log << "init and goal";
        } else if (compute_init_distances) {
            log << "init";
        } else if (compute_goal_distances) {
            log << "goal";
        }
        log << " distances";
    }

    if (compute_init_distances) {
        init_distances.resize(num_states, INFINITE_VALUE);
        Distances::compute_init_distances();
    }

    if (compute_goal_distances) {
        goal_distances.resize(num_states, INFINITE_VALUE);
        Distances::compute_goal_distances();
    }
}

void Distances::apply_abstraction(
    const StateEquivalenceRelation& state_equivalence_relation,
    bool compute_init_distances,
    bool compute_goal_distances,
    utils::LogProxy& log)
{
    if (compute_init_distances) {
        assert(are_init_distances_computed());
        assert(state_equivalence_relation.size() < init_distances.size());
    }

    if (compute_goal_distances) {
        assert(are_goal_distances_computed());
        assert(state_equivalence_relation.size() < goal_distances.size());
    }

    const int new_num_states = state_equivalence_relation.size();
    vector<value_t> new_init_distances;
    vector<value_t> new_goal_distances;
    if (compute_init_distances) {
        new_init_distances.resize(new_num_states, DISTANCE_UNKNOWN);
    }

    if (compute_goal_distances) {
        new_goal_distances.resize(new_num_states, DISTANCE_UNKNOWN);
    }

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const StateEquivalenceClass& state_eqv_class =
            state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto pos = state_eqv_class.begin();
        value_t new_init_dist = -1;
        value_t new_goal_dist = -1;
        if (compute_init_distances) {
            new_init_dist = init_distances[*pos];
        }

        if (compute_goal_distances) {
            new_goal_dist = goal_distances[*pos];
        }

        auto distance_different = [=, this](int state) {
            return (compute_init_distances &&
                    init_distances[state] != new_init_dist) ||
                   (compute_goal_distances &&
                    goal_distances[state] != new_goal_dist);
        };

        if (std::any_of(++pos, state_eqv_class.end(), distance_different)) {
            // Not J*-preserving -> recompute
            if (log.is_at_least_verbose()) {
                log << transition_system.tag()
                    << "simplification was not f-preserving!" << endl;
            }

            init_distances.clear();
            goal_distances.clear();
            init_distances_computed = false;
            goal_distances_computed = false;
            compute_distances(
                compute_init_distances,
                compute_goal_distances,
                log);
            return;
        }

        if (compute_init_distances) {
            new_init_distances[new_state] = new_init_dist;
        }
        if (compute_goal_distances) {
            new_goal_distances[new_state] = new_goal_dist;
        }
    }

    init_distances = std::move(new_init_distances);
    goal_distances = std::move(new_goal_distances);
}

void Distances::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        if (are_init_distances_computed()) {
            log << "Init distances: ";
            for (size_t i = 0; i < init_distances.size(); ++i) {
                log << i << ": " << init_distances[i];
                if (i != init_distances.size() - 1) {
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
