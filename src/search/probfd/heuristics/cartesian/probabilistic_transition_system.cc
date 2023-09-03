#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/probabilistic_transition.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>
#include <map>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

static vector<vector<FactPair>>
get_preconditions_by_operator(const ProbabilisticOperatorsProxy& ops)
{
    vector<vector<FactPair>> preconditions_by_operator;
    preconditions_by_operator.reserve(ops.size());
    for (auto op : ops) {
        vector<FactPair> preconditions =
            ::task_properties::get_fact_pairs(op.get_preconditions());
        sort(preconditions.begin(), preconditions.end());
        preconditions_by_operator.push_back(std::move(preconditions));
    }
    return preconditions_by_operator;
}

static vector<vector<FactPair>>
get_postconditions_per_outcome(const ProbabilisticOperatorProxy& op)
{
    vector<vector<FactPair>> postconditions;

    auto outcomes = op.get_outcomes();
    postconditions.reserve(outcomes.size());

    for (auto outcome : outcomes) {
        // Use map to obtain sorted postconditions.
        map<int, int> var_to_post;
        for (FactProxy fact : op.get_preconditions()) {
            var_to_post[fact.get_variable().get_id()] = fact.get_value();
        }
        for (auto effect : outcome.get_effects()) {
            FactPair fact = effect.get_fact().get_pair();
            var_to_post[fact.var] = fact.value;
        }
        vector<FactPair>& outcome_postconditions =
            postconditions.emplace_back();
        outcome_postconditions.reserve(var_to_post.size());
        for (const pair<const int, int>& fact : var_to_post) {
            outcome_postconditions.emplace_back(fact.first, fact.second);
        }
    }
    return postconditions;
}

static vector<vector<vector<FactPair>>>
get_postconditions_by_operator_and_outcome(
    const ProbabilisticOperatorsProxy& ops)
{
    vector<vector<vector<FactPair>>> postconditions;
    postconditions.reserve(ops.size());
    for (const ProbabilisticOperatorProxy op : ops) {
        postconditions.push_back(get_postconditions_per_outcome(op));
    }
    return postconditions;
}

static vector<vector<value_t>> get_probabilities_by_operator_and_outcome(
    const ProbabilisticOperatorsProxy& ops)
{
    vector<vector<value_t>> probabilities;
    probabilities.reserve(ops.size());
    for (const ProbabilisticOperatorProxy op : ops) {
        const auto outcomes = op.get_outcomes();
        auto& outcome_probabilities = probabilities.emplace_back();
        outcome_probabilities.reserve(outcomes.size());
        for (const ProbabilisticOutcomeProxy outcome : outcomes) {
            outcome_probabilities.push_back(outcome.get_probability());
        }
    }
    return probabilities;
}

static int lookup_value(const vector<FactPair>& facts, int var)
{
    assert(is_sorted(facts.begin(), facts.end()));
    for (const FactPair& fact : facts) {
        if (fact.var == var) {
            return fact.value;
        } else if (fact.var > var) {
            return UNDEFINED;
        }
    }
    return UNDEFINED;
}

ProbabilisticTransitionSystem::ProbabilisticTransitionSystem(
    const ProbabilisticOperatorsProxy& ops)
    : preconditions_by_operator(get_preconditions_by_operator(ops))
    , postconditions_by_operator_and_outcome(
          get_postconditions_by_operator_and_outcome(ops))
    , probabilities_by_operator_and_outcome(
          get_probabilities_by_operator_and_outcome(ops))
{
    construct_trivial_abstraction(ops);
}

int ProbabilisticTransitionSystem::get_precondition_value(int op_id, int var)
    const
{
    return lookup_value(preconditions_by_operator[op_id], var);
}

int ProbabilisticTransitionSystem::get_postcondition_value(
    int op_id,
    int eff_id,
    int var) const
{
    return lookup_value(
        postconditions_by_operator_and_outcome[op_id][eff_id],
        var);
}

size_t ProbabilisticTransitionSystem::get_num_operator_outcomes(int op_id) const
{
    return postconditions_by_operator_and_outcome[op_id].size();
}

void ProbabilisticTransitionSystem::enlarge_vectors_by_one()
{
    outgoing_by_id.emplace_back();
    partial_loops_by_id.emplace_back();
    outgoing_proxies.emplace_back();
    partial_loop_proxies.emplace_back();
    incoming_by_id.emplace_back();
    loops_by_id.emplace_back();
}

void ProbabilisticTransitionSystem::construct_trivial_abstraction(
    const ProbabilisticOperatorsProxy& ops)
{
    assert(get_num_states() == 0);
    enlarge_vectors_by_one();
    assert(get_num_states() == 1);

    for (const auto& op : ops) {
        loops_by_id[0].emplace_back(op.get_id());
    }

    num_loops += ops.size();
}

void ProbabilisticTransitionSystem::add_transition(
    int src_id,
    int op_id,
    std::vector<int> target_ids)
{
    assert(!utils::contains(target_ids, src_id));

    auto& t = outgoing_by_id[src_id].emplace_back(op_id, std::move(target_ids));

    TransitionProxy& proxy = proxy_nodes.emplace_back(&t, src_id, false);
    outgoing_proxies[src_id].emplace_back(&proxy);

    std::unordered_set<int> seen;
    for (int target_id : t.target_ids) {
        if (seen.insert(target_id).second) {
            incoming_by_id[target_id].push_back(&proxy);
        }
    }
}

void ProbabilisticTransitionSystem::add_partial_loop(
    int src_id,
    int op_id,
    std::vector<int> target_ids)
{
    assert(utils::contains(target_ids, src_id));

    auto& t =
        partial_loops_by_id[src_id].emplace_back(op_id, std::move(target_ids));

    TransitionProxy& proxy = proxy_nodes.emplace_back(&t, src_id, true);
    partial_loop_proxies[src_id].emplace_back(&proxy);

    std::unordered_set<int> seen;
    for (int target_id : t.target_ids) {
        if (target_id != src_id && seen.insert(target_id).second) {
            incoming_by_id[target_id].push_back(&proxy);
        }
    }
}

void ProbabilisticTransitionSystem::add_loop(int src_id, int op_id)
{
    loops_by_id[src_id].emplace_back(op_id);
    ++num_loops;
}

auto ProbabilisticTransitionSystem::zip_post(auto& targets, int op_id, int var)
    const
{
    return std::views::iota(0U, targets.size()) |
           std::views::transform([&, op_id, var](size_t i) {
               return std::make_pair(
                   std::ref(targets[i]),
                   get_postcondition_value(op_id, i, var));
           });
}

namespace {
auto undefined_precondition_rewire(
    auto&& target_post,
    int var,
    const AbstractStates& states,
    const AbstractState& v1,
    const AbstractState& v2,
    int v1_id,
    int v2_id,
    bool v1_prevail_init,
    bool v2_prevail_init)
{
    bool has_prevail = false;
    bool v1_prevail = v1_prevail_init;
    bool v2_prevail = v2_prevail_init;
    bool v1_target = false;
    bool v2_target = false;

    for (auto [target, post] : target_post) {
        if (target != v1_id) {
            if (post != UNDEFINED) continue;
            const AbstractState& v = *states[target];
            v1_prevail = v1_prevail && v.intersects(v1, var);
            v2_prevail = v2_prevail && v.intersects(v2, var);
        } else if (post == UNDEFINED) {
            has_prevail = true;
            target = UNDEFINED;
        } else if (v1.contains(var, post)) {
            v1_target = true;
            target = v1_id;
        } else {
            assert(v2.contains(var, post));
            v2_target = true;
            target = v2_id;
        }
    }

    return std::make_tuple(
        has_prevail,
        v1_prevail,
        v2_prevail,
        v1_target,
        v2_target);
}

auto defined_precondition_rewire(
    auto&& target_post,
    int var,
    const AbstractState& v1,
    [[maybe_unused]] const AbstractState& v2,
    int v1_id,
    int v2_id)
{
    bool v1_target = false;
    bool v2_target = false;

    for (auto [target, post] : target_post) {
        if (target != v1_id) continue;

        if (v1.contains(var, post)) {
            v1_target = true;
            target = v1_id;
        } else {
            assert(v2.contains(var, post));
            v2_target = true;
            target = v2_id;
        }
    }

    return std::make_pair(v1_target, v2_target);
}

void rewire_prevail_targets(std::vector<int>& targets, int rewire_to)
{
    std::ranges::replace(targets, UNDEFINED, rewire_to);
}

auto get_rewired_prevail_copy(std::vector<int> targets, int rewire_to)
{
    rewire_prevail_targets(targets, rewire_to);
    return targets;
}
} // namespace

void ProbabilisticTransitionSystem::rewire_incoming_transitions(
    const AbstractStates& states,
    const AbstractState& v1,
    const AbstractState& v2,
    int var)
{
    int v1_id = v1.get_id();
    int v2_id = v2.get_id();

    auto old_incoming = std::move(incoming_by_id[v1_id]);

    for (TransitionProxy* proxy : old_incoming) {
        ProbabilisticTransition& transition = *proxy->transition;
        int op_id = transition.op_id;

        // Note: Targets are updated in-place. If rewiring produces two
        // transitions, then the second one is added as a new transition, while
        // the first one is an in-place update.
        std::vector<int>& target_ids = transition.target_ids;

        const int u_id = proxy->source_id;
        const AbstractState& u = *states[u_id];

        assert(u_id != v1_id);
        assert(utils::contains(target_ids, v1_id));
        assert(proxy->partial_loop == utils::contains(target_ids, u_id));

        int pre = get_precondition_value(op_id, var);
        if (pre == UNDEFINED) {
            auto [has_prevail, v1_prevail, v2_prevail, v1_target, v2_target] =
                undefined_precondition_rewire(
                    zip_post(target_ids, op_id, var),
                    var,
                    states,
                    v1,
                    v2,
                    v1_id,
                    v2_id,
                    u.intersects(v1, var),
                    u.intersects(v2, var));

            if (has_prevail) {
                if (v1_prevail) {
                    v1_target = true;

                    if (v2_prevail) {
                        // Add the new second transition to the transition list.
                        if (proxy->partial_loop) {
                            add_partial_loop(
                                u_id,
                                op_id,
                                get_rewired_prevail_copy(target_ids, v2_id));
                        } else {
                            add_transition(
                                u_id,
                                op_id,
                                get_rewired_prevail_copy(target_ids, v2_id));
                        }
                    }
                    rewire_prevail_targets(target_ids, v1_id);
                } else {
                    v2_target = true;
                    rewire_prevail_targets(target_ids, v2_id);
                }
            }

            if (v1_target) {
                // Transition is incoming for v1.
                incoming_by_id[v1_id].push_back(proxy);
                assert(utils::contains(transition.target_ids, v1_id));
            }

            if (v2_target) {
                // Transition is incoming for v2.
                incoming_by_id[v2_id].push_back(proxy);
                assert(utils::contains(transition.target_ids, v2_id));
            }
        } else {
            const auto [v1_target, v2_target] = defined_precondition_rewire(
                zip_post(target_ids, op_id, var),
                var,
                v1,
                v2,
                v1_id,
                v2_id);

            assert(v1_target || v2_target);

            if (v1_target) incoming_by_id[v1_id].push_back(proxy);
            if (v2_target) incoming_by_id[v2_id].push_back(proxy);
        }
    }
}

void ProbabilisticTransitionSystem::rewire_outgoing_transitions(
    const AbstractStates& states,
    const AbstractState& v1,
    const AbstractState& v2,
    int var)
{
    int v1_id = v1.get_id();
    int v2_id = v2.get_id();

    auto old_outgoing = std::move(outgoing_by_id[v1_id]);
    auto old_outgoing_refs = std::move(outgoing_proxies[v1_id]);

    auto push = [&](auto& transition, auto* proxy, int source_id) {
        auto& t = outgoing_by_id[source_id].emplace_back(std::move(transition));
        proxy->transition = &t;
        outgoing_proxies[source_id].emplace_back(proxy);
    };

    auto z = std::views::zip(old_outgoing, old_outgoing_refs);
    for (const auto& [transition, proxy] : z) {
        int op_id = transition.op_id;
        std::vector<int>& target_ids = transition.target_ids;

        assert(!utils::contains(target_ids, v1_id));
        assert(proxy->source_id == v1_id);

        int pre = get_precondition_value(op_id, var);

        if (pre == UNDEFINED) {
            bool v1_prevail = true;
            bool v2_prevail = true;

            for (auto [target, post] : zip_post(target_ids, op_id, var)) {
                if (post != UNDEFINED) continue;
                const AbstractState& v = *states[target];
                v1_prevail = v1_prevail && v.intersects(v1, var);
                v2_prevail = v2_prevail && v.intersects(v2, var);
            }

            if (v1_prevail) {
                if (v2_prevail) {
                    // Copy transition to v2.
                    add_transition(v2_id, op_id, target_ids);
                }

                // Transition is still outgoing for v1.
                push(transition, proxy, v1_id);
            } else {
                // Transition is now outgoing for v2.
                proxy->source_id = v2_id;
                push(transition, proxy, v2_id);
            }
        } else if (v1.contains(var, pre)) {
            // Transition is still outgoing for v1.
            push(transition, proxy, v1_id);
        } else {
            assert(v2.contains(var, pre));

            // Transition is now outgoing for v2.
            proxy->source_id = v2_id;
            push(transition, proxy, v2_id);
        }
    }
}

void ProbabilisticTransitionSystem::rewire_partial_loops(
    const AbstractStates& states,
    const AbstractState& v1,
    const AbstractState& v2,
    int var)
{
    int v1_id = v1.get_id();
    int v2_id = v2.get_id();

    auto old_outgoing = std::move(partial_loops_by_id[v1_id]);
    auto old_outgoing_refs = std::move(partial_loop_proxies[v1_id]);

    auto push_partial_loop = [&](auto& transition, auto* proxy, int source_id) {
        assert(utils::contains(transition.target_ids, source_id));

        auto& t =
            partial_loops_by_id[source_id].emplace_back(std::move(transition));
        proxy->transition = &t;
        partial_loop_proxies[source_id].push_back(proxy);
    };

    auto push_outgoing = [&](auto& transition, auto* proxy, int source_id) {
        assert(!utils::contains(transition.target_ids, source_id));

        auto& t = outgoing_by_id[source_id].emplace_back(std::move(transition));
        proxy->transition = &t;
        proxy->partial_loop = false;
        outgoing_proxies[source_id].push_back(proxy);
    };

    auto z = std::views::zip(old_outgoing, old_outgoing_refs);
    for (const auto& [transition, proxy] : z) {
        int op_id = transition.op_id;
        std::vector<int>& target_ids = transition.target_ids;

        int& u_id = proxy->source_id;

        assert(u_id == v1_id);
        assert(utils::contains(target_ids, v1_id));

        int pre = get_precondition_value(op_id, var);
        if (pre == UNDEFINED) {
            auto [has_prevail, v1_prevail, v2_prevail, v1_target, v2_target] =
                undefined_precondition_rewire(
                    zip_post(target_ids, op_id, var),
                    var,
                    states,
                    v1,
                    v2,
                    v1_id,
                    v2_id,
                    true,
                    true);

            if (has_prevail) {
                if (v1_prevail) {
                    v1_target = true;

                    if (v2_prevail) {
                        // Add the new second transition to the transition list.
                        add_partial_loop(
                            v2_id,
                            op_id,
                            get_rewired_prevail_copy(target_ids, v2_id));
                    }
                    rewire_prevail_targets(target_ids, v1_id);
                } else {
                    v2_target = true;
                    rewire_prevail_targets(target_ids, v2_id);
                }
            }

            assert(v1_target || v2_target);

            if (v1_prevail) {
                if (v1_target) {
                    // Transition is still partial loop for v1.
                    push_partial_loop(transition, proxy, v1_id);

                    if (v2_target) {
                        // Transition is now incoming for v2.
                        incoming_by_id[v2_id].push_back(proxy);
                    }
                } else {
                    // Transition is now outgoing for v1, incoming for v2.
                    push_outgoing(transition, proxy, v1_id);
                    incoming_by_id[v2_id].push_back(proxy);
                }
            } else {
                // Rewire source state
                u_id = v2_id;

                if (v2_target) {
                    // Transition is now partial loop for v2.
                    push_partial_loop(transition, proxy, v2_id);

                    if (v1_target) {
                        // Transition is now incoming for v1.
                        incoming_by_id[v1_id].push_back(proxy);
                    }
                } else {
                    // Transition is now outgoing for v2, incoming for v1.
                    push_outgoing(transition, proxy, v2_id);
                    incoming_by_id[v1_id].push_back(proxy);
                }
            }
        } else {
            const auto [v1_target, v2_target] = defined_precondition_rewire(
                zip_post(target_ids, op_id, var),
                var,
                v1,
                v2,
                v1_id,
                v2_id);

            assert(v1_target || v2_target);

            const int pre = get_precondition_value(op_id, var);

            if (v2.contains(var, pre)) {
                // Rewire source state
                u_id = v2_id;

                if (v2_target) {
                    // Transition is now partial loop for v2.
                    push_partial_loop(transition, proxy, v2_id);

                    if (v1_target) {
                        // Transition is now incoming for v1.
                        incoming_by_id[v1_id].push_back(proxy);
                    }
                } else {
                    // Transition is now outgoing for v2, incoming for v1.
                    push_outgoing(transition, proxy, v2_id);
                    incoming_by_id[v1_id].push_back(proxy);
                }
            } else if (v1_target) {
                // Transition is still partial loop for v1.
                push_partial_loop(transition, proxy, v1_id);

                if (v2_target) {
                    // Transition is now incoming for v2.
                    incoming_by_id[v2_id].push_back(proxy);
                }
            } else {
                // Transition is now outgoing for v1, incoming for v2.
                push_outgoing(transition, proxy, v1_id);
                incoming_by_id[v2_id].push_back(proxy);
            }
        }
    }
}

void ProbabilisticTransitionSystem::rewire_loops(
    const AbstractState& v1,
    const AbstractState& v2,
    int var)
{
    /* State v has been split into v1 and v2. Now for all self-loops
       v->v we need to add one or two of the transitions v1->v1, v1->v2,
       v2->v1 and v2->v2. */
    int v1_id = v1.get_id();
    int v2_id = v2.get_id();

    auto old_loops = std::move(loops_by_id[v1_id]);

    for (int op_id : old_loops) {
        const int pre = get_precondition_value(op_id, var);
        const size_t num_outcomes = get_num_operator_outcomes(op_id);

        if (pre == UNDEFINED) {
            // op starts in v1
            bool v1_set = false;
            bool v2_set = false;
            bool has_prevail = false;

            std::vector<int> target_ids_v1;
            std::vector<int> target_ids_v2;
            target_ids_v1.reserve(num_outcomes);
            target_ids_v2.reserve(num_outcomes);

            for (size_t i = 0; i != num_outcomes; ++i) {
                int post = get_postcondition_value(op_id, i, var);

                if (post == UNDEFINED) {
                    target_ids_v1.push_back(v1_id);
                    target_ids_v2.push_back(v2_id);
                    has_prevail = true;
                } else if (v1.contains(var, post)) {
                    // effect must end in v1.
                    target_ids_v1.push_back(v1_id);
                    target_ids_v2.push_back(v1_id);
                    v1_set = true;
                } else {
                    // effect must end in v2.
                    target_ids_v1.push_back(v2_id);
                    target_ids_v2.push_back(v2_id);
                    v2_set = true;
                }
            }

            if (has_prevail) {
                if (v1_set) {
                    if (v2_set) {
                        // some effects go to v1, some to v2.
                        add_partial_loop(
                            v1_id,
                            op_id,
                            std::move(target_ids_v1));
                        add_partial_loop(
                            v2_id,
                            op_id,
                            std::move(target_ids_v2));
                    } else {
                        // all non-prevail effects go to v1
                        add_loop(v1_id, op_id);
                        add_partial_loop(
                            v2_id,
                            op_id,
                            std::move(target_ids_v2));
                    }
                } else {
                    if (v2_set) {
                        // all non-prevail effects go to v2
                        add_partial_loop(
                            v1_id,
                            op_id,
                            std::move(target_ids_v1));
                        add_loop(v2_id, op_id);
                    } else {
                        // all effects prevail
                        add_loop(v1_id, op_id);
                        add_loop(v2_id, op_id);
                    }
                }
            } else {
                assert(v1_set || v2_set);

                if (!v2_set) {
                    // all effects go to v1.
                    add_loop(v1_id, op_id);
                    add_transition(v2_id, op_id, std::move(target_ids_v2));
                } else if (!v1_set) {
                    // all effects go to v2.
                    add_transition(v1_id, op_id, std::move(target_ids_v1));
                    add_loop(v2_id, op_id);
                } else {
                    // some effects go to v1, some to v2.
                    add_partial_loop(v1_id, op_id, std::move(target_ids_v1));
                    add_partial_loop(v2_id, op_id, std::move(target_ids_v2));
                }
            }
        } else if (v1.contains(var, pre)) {
            // op starts in v1
            bool v1_set = false;
            bool v2_set = false;

            std::vector<int> target_ids;
            target_ids.reserve(num_outcomes);

            for (size_t i = 0; i != num_outcomes; ++i) {
                int post = get_postcondition_value(op_id, i, var);
                assert(post != UNDEFINED);

                if (v1.contains(var, post)) {
                    // effect must end in v1.
                    target_ids.push_back(v1_id);
                    v1_set = true;
                } else {
                    // effect must end in v2.
                    target_ids.push_back(v2_id);
                    v2_set = true;
                }
            }

            if (!v2_set) {
                // uniform v1 self-loop.
                add_loop(v1_id, op_id);
            } else if (!v1_set) {
                // all effects go to v2.
                add_transition(v1_id, op_id, std::move(target_ids));
            } else {
                // some effects go to v1, some to v2.
                add_partial_loop(v1_id, op_id, std::move(target_ids));
            }
        } else {
            // op starts in v2
            bool v1_set = false;
            bool v2_set = false;

            std::vector<int> target_ids;
            target_ids.reserve(num_outcomes);

            for (size_t i = 0; i != num_outcomes; ++i) {
                int post = get_postcondition_value(op_id, i, var);
                assert(post != UNDEFINED);

                if (v1.contains(var, post)) {
                    // effect must end in v1.
                    target_ids.push_back(v1_id);
                    v1_set = true;
                } else {
                    // effect must end in v2.
                    target_ids.push_back(v2_id);
                    v2_set = true;
                }
            }

            if (!v1_set) {
                // uniform v2 self-loop.
                add_loop(v2_id, op_id);
            } else if (!v2_set) {
                // all effects go to v1.
                add_transition(v2_id, op_id, std::move(target_ids));
            } else {
                // some effects go to v1, some to v2.
                add_partial_loop(v2_id, op_id, std::move(target_ids));
            }
        }
    }

    num_loops -= old_loops.size();
}

int ProbabilisticTransitionSystem::check_num_non_loop_transitions_after_rewire(
    const AbstractStates& states,
    const CartesianSet& v1,
    const CartesianSet& v2,
    int split_id,
    int var) const
{
    int n = 0;

    for (TransitionProxy* proxy : incoming_by_id[split_id]) {
        const ProbabilisticTransition& transition = *proxy->transition;

        int op_id = transition.op_id;
        const std::vector<int>& target_ids = transition.target_ids;

        const int u_id = proxy->source_id;
        const AbstractState& u = *states[u_id];

        if (get_precondition_value(op_id, var) == UNDEFINED) {
            if (!u.intersects(v1, var)) continue;
            if (!u.intersects(v2, var)) continue;

            bool has_prevail = false;

            for (auto [target, post] : zip_post(target_ids, op_id, var)) {
                if (target != split_id) {
                    if (post != UNDEFINED) continue;
                    const AbstractState& v = *states[target];
                    if (!v.intersects(v1, var) || !v.intersects(v2, var))
                        goto skip;
                } else if (post == UNDEFINED) {
                    has_prevail = true;
                }
            }

            if (has_prevail) {
                ++n;
            }
        }

    skip:;
    }

    for (const auto& transition : outgoing_by_id[split_id]) {
        int op_id = transition.op_id;
        const std::vector<int>& target_ids = transition.target_ids;

        if (get_precondition_value(op_id, var) == UNDEFINED) {
            for (auto [target, post] : zip_post(target_ids, op_id, var)) {
                if (post != UNDEFINED) continue;
                const AbstractState& v = *states[target];
                if (!v.intersects(v1, var) || !v.intersects(v2, var))
                    goto skip2;
            }

            ++n;
        }

    skip2:;
    }

    for (const auto& transition : partial_loops_by_id[split_id]) {
        int op_id = transition.op_id;
        const std::vector<int>& target_ids = transition.target_ids;

        if (get_precondition_value(op_id, var) == UNDEFINED) {
            bool has_prevail = false;

            for (auto [target, post] : zip_post(target_ids, op_id, var)) {
                if (target != split_id) {
                    if (post != UNDEFINED) continue;
                    const AbstractState& v = *states[target];
                    if (!v.intersects(v1, var) || !v.intersects(v2, var))
                        goto skip3;
                } else if (post == UNDEFINED) {
                    has_prevail = true;
                }
            }

            if (has_prevail) {
                ++n;
            }
        }

    skip3:;
    }

    for (int op_id : loops_by_id[split_id]) {
        if (get_precondition_value(op_id, var) == UNDEFINED) {
            ++n;
        }
    }

    return n + get_num_non_loops();
}

int ProbabilisticTransitionSystem::
    estimate_num_non_loop_transitions_after_rewire(int split_id) const
{
    // A simple upper bound can be calculated by assuming every transition
    // results in two rewired transitions.
    return get_num_non_loops() + loops_by_id[split_id].size() +
           partial_loops_by_id[split_id].size() +
           outgoing_by_id[split_id].size() + incoming_by_id[split_id].size();
}

void ProbabilisticTransitionSystem::rewire(
    const AbstractStates& states,
    const AbstractState& v1,
    const AbstractState& v2,
    int var)
{
    // Make space for new transitions.
    enlarge_vectors_by_one();

    // Remove old transitions and add new transitions.
    rewire_incoming_transitions(states, v1, v2, var);
    rewire_outgoing_transitions(states, v1, v2, var);
    rewire_partial_loops(states, v1, v2, var);
    rewire_loops(v1, v2, var);
}

value_t
ProbabilisticTransitionSystem::get_probability(int op_index, int eff_index)
    const
{
    return probabilities_by_operator_and_outcome[op_index][eff_index];
}

const std::deque<std::deque<ProbabilisticTransition>>&
ProbabilisticTransitionSystem::get_outgoing_transitions() const
{
    return outgoing_by_id;
}

const std::deque<std::deque<ProbabilisticTransition>>&
ProbabilisticTransitionSystem::get_partial_loops() const
{
    return partial_loops_by_id;
}

const std::vector<std::vector<int>>&
ProbabilisticTransitionSystem::get_loops() const
{
    return loops_by_id;
}

int ProbabilisticTransitionSystem::get_num_states() const
{
    assert(incoming_by_id.size() == outgoing_by_id.size());
    return outgoing_by_id.size();
}

int ProbabilisticTransitionSystem::get_num_operators() const
{
    return preconditions_by_operator.size();
}

int ProbabilisticTransitionSystem::get_num_non_loops() const
{
    return proxy_nodes.size();
}

int ProbabilisticTransitionSystem::get_num_loops() const
{
    return num_loops;
}

void ProbabilisticTransitionSystem::print_statistics(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Looping transitions: " << get_num_loops() << endl;
        log << "Non-looping transitions: " << get_num_non_loops() << endl;
    }
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd