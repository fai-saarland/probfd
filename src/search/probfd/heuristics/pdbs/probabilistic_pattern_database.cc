#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/value_type.h"

#include <algorithm>
#include <compare>
#include <deque>
#include <numeric>
#include <set>

namespace probfd {
namespace heuristics {
namespace pdbs {

ProbabilisticPatternDatabase::ProbabilisticPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern)
    : ranking_function_(task_proxy, std::move(pattern))
    , value_table(ranking_function_.num_states(), INFINITE_VALUE)
{
}

ProbabilisticPatternDatabase::ProbabilisticPatternDatabase(
    StateRankingFunction ranking_function)
    : ranking_function_(std::move(ranking_function))
    , value_table(ranking_function_.num_states(), INFINITE_VALUE)
{
}

const StateRankingFunction&
ProbabilisticPatternDatabase::get_abstract_state_mapper() const
{
    return ranking_function_;
}

unsigned int ProbabilisticPatternDatabase::num_states() const
{
    return ranking_function_.num_states();
}

bool ProbabilisticPatternDatabase::is_dead_end(const State& s) const
{
    return is_dead_end(get_abstract_state(s));
}

bool ProbabilisticPatternDatabase::is_dead_end(StateRank s) const
{
    return utils::contains(dead_ends_, StateID(s.id));
}

value_t ProbabilisticPatternDatabase::lookup(const State& s) const
{
    return lookup(get_abstract_state(s));
}

value_t ProbabilisticPatternDatabase::lookup(StateRank s) const
{
    return value_table[s.id];
}

StateRank ProbabilisticPatternDatabase::get_abstract_state(const State& s) const
{
    return ranking_function_.rank(s);
}

StateRank ProbabilisticPatternDatabase::get_abstract_state(
    const std::vector<int>& s) const
{
    return ranking_function_.rank(s);
}

const Pattern& ProbabilisticPatternDatabase::get_pattern() const
{
    return ranking_function_.get_pattern();
}

std::unique_ptr<AbstractPolicy>
ProbabilisticPatternDatabase::get_optimal_abstract_policy(
    ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard,
    bool use_cost) const
{
    using PredecessorList =
        std::vector<std::pair<StateRank, const AbstractOperator*>>;

    assert(!is_dead_end(state_space.initial_state_));

    AbstractPolicy* policy = new AbstractPolicy(ranking_function_.num_states());

    // return empty policy indicating unsolvable
    if (state_space.is_goal(state_space.initial_state_)) {
        return std::unique_ptr<AbstractPolicy>(policy);
    }

    std::map<StateRank, PredecessorList> predecessors;

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(state_space.initial_state_);
    closed.insert(state_space.initial_state_);

    std::vector<StateRank> goals;

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        // Skip dead-ends, the operator is irrelevant
        if (is_dead_end(s)) {
            continue;
        }

        const value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        // Select the greedy operators and add their successors
        for (const AbstractOperator* op : aops) {
            value_t op_value = use_cost ? op->cost : 0_vt;

            std::vector<StateRank> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                StateRank t = s + eff;
                op_value += prob * value_table[t.id];
                successors.push_back(t);
            }

            if (is_approx_equal(value, op_value)) {
                for (const StateRank& succ : successors) {
                    if (state_space.is_goal(succ)) {
                        goals.push_back(succ);
                    } else if (closed.insert(succ).second) {
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
        StateRank s = *it;

        std::swap(*it, open.back());
        open.pop_back();

        // Consider predecessors in random order
        rng->shuffle(predecessors[s]);

        for (const auto& [pstate, sel_op] : predecessors[s]) {
            if (closed.insert(pstate).second) {
                open.push_back(pstate);

                // Collect all equivalent greedy operators
                std::vector<const AbstractOperator*> aops;
                state_space.generate_applicable_actions(pstate.id, aops);

                std::vector<const AbstractOperator*> equivalent_operators;

                for (const AbstractOperator* op : aops) {
                    if (op->outcomes == sel_op->outcomes) {
                        equivalent_operators.push_back(op);
                    }
                }

                // If wildcard consider all, else randomly pick one
                if (wildcard) {
                    (*policy)[pstate].insert(
                        (*policy)[pstate].end(),
                        equivalent_operators.begin(),
                        equivalent_operators.end());
                } else {
                    (*policy)[pstate].push_back(
                        *rng->choose(equivalent_operators));
                }
            }
        }
    }

    return std::unique_ptr<AbstractPolicy>(policy);
}

std::unique_ptr<AbstractPolicy>
ProbabilisticPatternDatabase::get_optimal_abstract_policy_no_traps(
    ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard,
    bool use_cost) const
{
    AbstractPolicy* policy = new AbstractPolicy(ranking_function_.num_states());

    assert(lookup(state_space.initial_state_) != INFINITE_VALUE);

    if (state_space.is_goal(state_space.initial_state_)) {
        return std::unique_ptr<AbstractPolicy>(policy);
    }

    std::deque<StateRank> open;
    std::unordered_set<StateRank> closed;
    open.push_back(state_space.initial_state_);
    closed.insert(state_space.initial_state_);

    // Build the greedy policy graph
    while (!open.empty()) {
        StateRank s = open.front();
        open.pop_front();

        const value_t value = value_table[s.id];

        // Generate operators...
        std::vector<const AbstractOperator*> aops;
        state_space.generate_applicable_actions(s.id, aops);

        if (aops.empty()) {
            assert(value == INFINITE_VALUE);
            continue;
        }

        // Look at the (greedy) operators in random order.
        rng->shuffle(aops);

        const AbstractOperator* greedy_operator = nullptr;
        std::vector<StateRank> greedy_successors;

        // Select first greedy operator
        for (const AbstractOperator* op : aops) {
            value_t op_value = use_cost ? op->cost : 0;

            std::vector<StateRank> successors;

            for (const auto& [eff, prob] : op->outcomes) {
                StateRank t = s + eff;
                op_value += prob * value_table[t.id];
                successors.push_back(t);
            }

            if (is_approx_equal(value, op_value)) {
                greedy_operator = op;
                greedy_successors = std::move(successors);
                break;
            }
        }

        assert(greedy_operator != nullptr);

        // Generate successors
        for (const StateRank& succ : greedy_successors) {
            if (!state_space.is_goal(succ) && !closed.contains(succ)) {
                closed.insert(succ);
                open.push_back(succ);
            }
        }

        // Collect all equivalent greedy operators
        std::vector<const AbstractOperator*> equivalent_operators;

        for (const AbstractOperator* op : aops) {
            if (op->outcomes == greedy_operator->outcomes) {
                equivalent_operators.push_back(op);
            }
        }

        // If wildcard consider all, else randomly pick one
        if (wildcard) {
            (*policy)[s].insert(
                (*policy)[s].end(),
                equivalent_operators.begin(),
                equivalent_operators.end());
        } else {
            (*policy)[s].push_back(*rng->choose(equivalent_operators));
        }

        assert(!(*policy)[s].empty());
    }

    return std::unique_ptr<AbstractPolicy>(policy);
}

void ProbabilisticPatternDatabase::dump_graphviz(
    ProjectionStateSpace& state_space,
    const std::string& path,
    std::function<std::string(const StateRank&)> sts,
    AbstractCostFunction& costs,
    bool transition_labels) const
{
    using namespace engine_interfaces;

    ProbabilisticTaskProxy task_proxy(*tasks::g_root_task);
    AbstractOperatorToString op_names(task_proxy);

    auto ats = [=](const AbstractOperator* const& op) {
        return transition_labels ? op_names(op) : "";
    };

    std::ofstream out(path);

    graphviz::dump_state_space_dot_graph<StateRank, const AbstractOperator*>(
        out,
        state_space.initial_state_,
        &state_space,
        &costs,
        nullptr,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
