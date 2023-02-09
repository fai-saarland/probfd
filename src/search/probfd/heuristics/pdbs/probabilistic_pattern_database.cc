#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/value_utils.h"

#include <algorithm>
#include <compare>
#include <deque>
#include <numeric>
#include <set>

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
// Footprint used for detecting duplicate operators.
struct ProgressionOperatorFootprint {
    long long int precondition_hash;
    std::vector<ItemProbabilityPair<StateRank>> successors;

    ProgressionOperatorFootprint(
        long long int precondition_hash,
        const AbstractOperator& op)
        : precondition_hash(precondition_hash)
        , successors(op.outcomes.begin(), op.outcomes.end())
    {
        std::ranges::sort(successors);
    }

    friend auto operator<=>(
        const ProgressionOperatorFootprint& a,
        const ProgressionOperatorFootprint& b) = default;
};

struct OutcomeInfo {
    StateRank base_effect = StateRank(0);
    std::vector<int> missing_pres;

    friend auto
    operator<=>(const OutcomeInfo& a, const OutcomeInfo& b) = default;
};

} // namespace

ProjectionStateSpace::ProjectionStateSpace(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    bool operator_pruning)
    : initial_state_(ranking_function.rank(task_proxy.get_initial_state()))
    , match_tree_(task_proxy, ranking_function.get_pattern())
    , goal_state_flags_(ranking_function.num_states())
{
    const Pattern& pattern = ranking_function.get_pattern();

    const VariablesProxy variables = task_proxy.get_variables();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy task_goals = task_proxy.get_goals();

    abstract_operators_.reserve(operators.size());

    std::set<ProgressionOperatorFootprint> duplicate_set;

    std::vector<int> pdb_indices(variables.size(), -1);

    for (size_t i = 0; i < pattern.size(); ++i) {
        pdb_indices[pattern[i]] = i;
    }

    // Construct partial assignment ranking function for operator pruning
    std::vector<long long int> partial_multipliers;
    if (operator_pruning) {
        partial_multipliers.reserve(pattern.size());

        int p = 1;
        partial_multipliers.push_back(1);

        for (size_t i = 1; i != pattern.size(); ++i) {
            p *= ranking_function.get_domain_size(i - 1) + 1;
            partial_multipliers.push_back(p);
        }
    }

    // Generate the abstract operators for each probabilistic operator
    for (const ProbabilisticOperatorProxy& op : operators) {
        const int operator_id = op.get_id();
        const int cost = op.get_cost();

        // Precondition partial state and partial state to enumerate
        // effect values not appearing in precondition
        std::vector<FactPair> local_precondition;

        for (FactProxy fact : op.get_preconditions()) {
            const int pre_var = fact.get_variable().get_id();
            const int pdb_index = pdb_indices[pre_var];

            if (pdb_index != -1) {
                local_precondition.emplace_back(pdb_index, fact.get_value());
            }
        }

        std::vector<FactPair> vars_eff_not_pre;

        // Info about each probabilistic outcome
        Distribution<OutcomeInfo> outcomes;

        // Collect info about the outcomes
        for (const ProbabilisticOutcomeProxy& out : op.get_outcomes()) {
            OutcomeInfo info;

            std::vector<FactPair> local_effect;

            for (ProbabilisticEffectProxy effect : out.get_effects()) {
                FactProxy fact = effect.get_fact();
                const int eff_var = fact.get_variable().get_id();
                const int pdb_index = pdb_indices[eff_var];
                if (pdb_index != -1) {
                    local_effect.emplace_back(pdb_index, fact.get_value());
                }
            }

            for (const auto& [var, val] : local_effect) {
                auto pre_it = std::ranges::lower_bound(
                    local_precondition,
                    var,
                    std::ranges::less(),
                    &FactPair::var);

                int val_change = val;

                if (pre_it == local_precondition.end() || pre_it->var != var) {
                    vars_eff_not_pre.emplace_back(var, 0);
                    info.missing_pres.push_back(var);
                } else {
                    val_change -= pre_it->value;
                }

                info.base_effect += ranking_function.from_fact(var, val_change);
            }

            outcomes.add_unique(std::move(info), out.get_probability());
        }

        utils::sort_unique(vars_eff_not_pre);

        // We enumerate all values for variables that are not part of
        // the precondition but in an effect. Depending on the value of the
        // variable, the value change caused by the abstract operator would
        // be different, hence we generate on operator for each state where
        // enabled.
        auto ran =
            ranking_function.partial_assignments(std::move(vars_eff_not_pre));

        for (const std::vector<FactPair>& values : ran) {
            // Generate the progression operator
            AbstractOperator new_op(operator_id, cost);

            for (const auto& [info, prob] : outcomes) {
                const auto& [base_effect, missing_pres] = info;
                auto a =
                    ranking_function.from_values_partial(missing_pres, values);

                new_op.outcomes.add_unique(base_effect - a, prob);
            }

            // Construct the precondition by merging the original
            // precondition partial state with the partial state for the
            // non-precondition effects of this iteration
            std::vector<FactPair> precondition;
            precondition.reserve(local_precondition.size() + values.size());

            std::ranges::merge(
                local_precondition,
                values,
                std::back_inserter(precondition));

            if (operator_pruning) {
                // Generate a rank for the precondition to check for
                // duplicates
                long long int pre_rank = 0;
                for (const auto& [var, val] : precondition) {
                    // Missing preconditions are -1, so we add 1 to adjust to
                    // the range [0, d + 1] where d is the domain size
                    pre_rank += partial_multipliers[var] * (val + 1);
                }

                if (!duplicate_set.emplace(pre_rank, new_op).second) {
                    continue;
                }
            }

            // Now add the progression operators to the match tree
            match_tree_.insert(
                ranking_function,
                abstract_operators_.size(),
                precondition);
            abstract_operators_.emplace_back(std::move(new_op));
        }
    }

    match_tree_.set_first_aop(abstract_operators_.data());

    std::vector<int> non_goal_vars;
    StateRank base(0);

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    const int num_goal_facts = task_goals.size();
    const int num_variables = pattern.size();

    for (int v = 0, w = 0; v != static_cast<int>(pattern.size());) {
        const int p_var = pattern[v];
        const FactProxy goal_fact = task_goals[w];
        const int g_var = goal_fact.get_variable().get_id();

        if (p_var < g_var) {
            non_goal_vars.push_back(v++);
        } else {
            if (p_var == g_var) {
                const int g_val = goal_fact.get_value();
                base.id += ranking_function.get_multiplier(v++) * g_val;
            }

            if (++w == num_goal_facts) {
                while (v < num_variables) {
                    non_goal_vars.push_back(v++);
                }
                break;
            }
        }
    }

    assert(non_goal_vars.size() != pattern.size()); // No goal no fun.

    auto goals = ranking_function.state_ranks(base, std::move(non_goal_vars));

    for (const auto& g : goals) {
        goal_state_flags_[g.id] = true;
    }
}

ProbabilisticPatternDatabase::ProbabilisticPatternDatabase(
    const ProbabilisticTaskProxy& task_proxy,
    Pattern pattern,
    value_t fill)
    : ranking_function_(task_proxy, std::move(pattern))
    , value_table(ranking_function_.num_states(), fill)
{
}

ProbabilisticPatternDatabase::ProbabilisticPatternDatabase(
    StateRankingFunction ranking_function,
    value_t fill)
    : ranking_function_(std::move(ranking_function))
    , value_table(ranking_function_.num_states(), fill)
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
    const ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard,
    bool use_cost) const
{
    using PredecessorList =
        std::vector<std::pair<StateRank, const AbstractOperator*>>;

    assert(!is_dead_end(state_space.initial_state_));

    AbstractPolicy* policy = new AbstractPolicy(ranking_function_.num_states());

    // return empty policy indicating unsolvable
    if (state_space.goal_state_flags_[state_space.initial_state_.id]) {
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
        state_space.match_tree_.get_applicable_operators(s, aops);

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
                    if (state_space.goal_state_flags_[succ.id]) {
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
                state_space.match_tree_.get_applicable_operators(pstate, aops);

                std::vector<const AbstractOperator*> equivalent_operators;

                for (const AbstractOperator* op : aops) {
                    if (op->outcomes.data() == sel_op->outcomes.data()) {
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
    const ProjectionStateSpace& state_space,
    const std::shared_ptr<utils::RandomNumberGenerator>& rng,
    bool wildcard,
    bool use_cost) const
{
    AbstractPolicy* policy = new AbstractPolicy(ranking_function_.num_states());

    assert(lookup(state_space.initial_state_) != INFINITE_VALUE);

    if (state_space.goal_state_flags_[state_space.initial_state_.id]) {
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
        state_space.match_tree_.get_applicable_operators(s, aops);

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
            if (!state_space.goal_state_flags_[succ.id] &&
                !closed.contains(succ)) {
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
    const ProjectionStateSpace& state_space,
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

    StateIDMap<StateRank> state_id_map;

    TransitionGenerator<const AbstractOperator*> transition_gen(
        state_id_map,
        state_space.match_tree_);

    std::ofstream out(path);

    graphviz::dump_state_space_dot_graph<StateRank, const AbstractOperator*>(
        out,
        state_space.initial_state_,
        &state_id_map,
        &transition_gen,
        &costs,
        nullptr,
        sts,
        ats,
        true);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
