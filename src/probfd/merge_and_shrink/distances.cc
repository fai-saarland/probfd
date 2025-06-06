#include "probfd/merge_and_shrink/distances.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/distribution.h"
#include "probfd/mdp.h"
#include "probfd/transition_tail.h"

#include "downward/utils/logging.h"
#include "probfd/merge_and_shrink/labels.h"

#include <cassert>
#include <deque>
#include <ranges>

#ifndef NDEBUG
#include <utility> // for std::cmp_equal
#endif

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

void compute_liveness(
    const TransitionSystem& transition_system,
    std::span<const value_t> goal_distances,
    std::vector<bool>& liveness,
    std::vector<int> queue)
{
    const int init_state = transition_system.get_init_state();

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
        const int state = queue.back();
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
    ExplicitMDP(const Labels& labels, const TransitionSystem& transition_system)
        : transitions_(transition_system.get_size())
        , goal_flags_(transition_system.get_size())
    {
        namespace vws = std::views;

        // Set up transitions
        for (const auto& label_info : transition_system.label_infos()) {
            const value_t label_cost = label_info.get_cost();
            const auto& probabilities = labels.get_label_probabilities(
                label_info.get_label_group().front());
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

void Distances::compute_distances(
    const Labels& labels,
    const TransitionSystem& transition_system,
    bool compute_liveness,
    utils::LogProxy& log,
    const Heuristic<int>& heuristic)
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
        log << "goal distances" << endl;
    }

    goal_distances.resize(num_states);
    compute_goal_distances(
        labels,
        transition_system,
        goal_distances,
        heuristic);
    goal_distances_computed = true;

    if (compute_liveness) {
        liveness.resize(num_states, false);
        std::ranges::fill(liveness, false);
        merge_and_shrink::compute_liveness(
            transition_system,
            goal_distances,
            liveness);
        liveness_computed = true;
    }
}

void Distances::apply_abstraction(
    const Labels& labels,
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

    const int new_num_states = state_equivalence_relation.size();
    vector<bool> new_liveness;
    vector new_goal_distances(new_num_states, DISTANCE_UNKNOWN);
    if (compute_liveness) {
        new_liveness.resize(new_num_states, false);
    }

    bool recompute_goal_distances = false;
    bool recompute_liveness = false;

    for (int new_state = 0; new_state < new_num_states; ++new_state) {
        const auto& state_eqv_class = state_equivalence_relation[new_state];
        assert(!state_eqv_class.empty());

        auto pos = state_eqv_class.begin();

        const value_t new_goal_dist = goal_distances[*pos];
        new_goal_distances[new_state] = new_goal_dist;

        // HACK: ad-hoc fp precision value used here that is more tolerant
        // than the default tolerance. Ideally, the shrink strategies
        // should return whether the returned abstraction is exact, so we
        // can skip this check altogether if that is the case.

        auto distance_different = [=, this](int state) {
            // HACK: ad-hoc fp precision value used here that is more tolerant
            // than the default tolerance. Ideally, the shrink strategies
            // should return whether the returned abstraction is exact, so we
            // can skip this check altogether if that is the case.
            return !is_approx_equal(goal_distances[state], new_goal_dist, 1e-3);
        };

        using std::views::drop;

        if (ranges::any_of(state_eqv_class | drop(1), distance_different)) {
            recompute_goal_distances = true;
            break;
        }

        if (compute_liveness) {
            const bool is_alive = liveness[*pos];
            new_liveness[new_state] = is_alive;

            auto liveness_different = [=, this](int state) {
                return liveness[state] != is_alive;
            };

            if (std::any_of(
                    std::next(pos),
                    state_eqv_class.end(),
                    liveness_different)) {
                recompute_liveness = true;
            }
        }
    }

    if (recompute_goal_distances) {
        // Not J*-preserving -> recompute
        if (log.is_at_least_verbose()) {
            log << transition_system.tag()
                << "simplification was not J*-preserving!" << endl;
        }
        liveness.clear();
        goal_distances.clear();
        liveness_computed = false;
        goal_distances_computed = false;
        compute_distances(labels, transition_system, compute_liveness, log);
        return;
    }

    if (recompute_liveness) {
        // J* preserving, but not alive preserving -> recompute
        if (log.is_at_least_verbose()) {
            log << transition_system.tag()
                << "simplification was not alive-preserving!" << endl;
        }
        const int num_states = transition_system.get_size();
        liveness.resize(num_states);
        std::ranges::fill(liveness, false);
        merge_and_shrink::compute_liveness(
            transition_system,
            goal_distances,
            liveness);
        liveness_computed = true;
        return;
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

void compute_goal_distances(
    const Labels& labels,
    const TransitionSystem& transition_system,
    std::span<value_t> distances,
    const Heuristic<int>& heuristic)
{
    using namespace algorithms::ta_topological_vi;

    assert(std::cmp_equal(distances.size(), transition_system.get_size()));

    ExplicitMDP explicit_mdp(labels, transition_system);

    TATopologicalValueIteration<int, const ProbabilisticTransition*> tatvi(
        0.0001,
        transition_system.get_size());

    std::ranges::fill(distances, DISTANCE_UNKNOWN);

    for (int i = 0; i != transition_system.get_size(); ++i) {
        if (!std::isnan(distances[i])) continue; // Already seen
        tatvi.solve(explicit_mdp, heuristic, i, distances);
    }
}

} // namespace probfd::merge_and_shrink
