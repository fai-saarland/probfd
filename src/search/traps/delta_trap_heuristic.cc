#include "delta_trap_heuristic.h"

#include "../algorithms/perfect_hashing_function.h"
#include "../algorithms/segmented_vector.h"
#include "../global_operator.h"
#include "../global_state.h"
#include "../globals.h"
#include "../option_parser.h"
#include "../partial_state.h"
#include "../plugin.h"
#include "../strips.h"
#include "../successor_generator.h"
#include "../utils/hash.h"
#include "../utils/system.h"
#include "../utils/timer.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <unordered_map>

namespace traps {

static Plugin<Heuristic>
    _plugin("delta_trap", options::parse<Heuristic, DeltaTrapHeuristic>);

static bool
is_delta_defined(const options::Options& opts)
{
    return !opts.get_list<std::shared_ptr<Heuristic>>("deltas").empty();
}

DeltaTrapHeuristic::DeltaTrapHeuristic(const options::Options& opts)
    : Heuristic(opts)
    , prune_transitions_(opts.get<bool>("prune_transitions"))
    , delta_prune_states_(
          is_delta_defined(opts) && opts.get<bool>("delta_prune_states"))
    , delta_prune_transitions_(
          is_delta_defined(opts) && opts.get<bool>("delta_prune_transitions"))
    , cache_delta_estimates_(
          is_delta_defined(opts) && opts.get<bool>("cache_delta_results"))
    , fact_map_(strips_utils::STRIPS::get_task())
#if TRAP_FORMULA_AS_UB_TREE
    , conjunctions_()
#else
    , conjunctions_(fact_map_->num_facts())
#endif
{
    std::cout << "Initializing Delta-trap heuristic ..." << std::endl;
    if (opts.get<int>("k") > 0) {
        std::vector<std::shared_ptr<Heuristic>> deltas =
            opts.get_list<std::shared_ptr<Heuristic>>("deltas");
        for (unsigned i = 0; i < deltas.size(); ++i) {
            if (!deltas[i]->supports_partial_state_evaluation()) {
                std::cerr << "Heuristics passed to Delta-trap heuristic must "
                             "support partial state evaluation!"
                          << std::endl;
                ::utils::exit_with(::utils::ExitCode::SEARCH_CRITICAL_ERROR);
            }
        }
        build_k_trap(
            std::min(
                static_cast<int>(g_variable_domain.size()), opts.get<int>("k")),
            deltas);
    }
}

void
DeltaTrapHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<int>("k", "", "0");
    parser.add_list_option<std::shared_ptr<Heuristic>>("deltas", "", "[]");
    parser.add_option<bool>("prune_transitions", "", "true");
    parser.add_option<bool>("delta_prune_transitions", "", "true");
    parser.add_option<bool>("delta_prune_states", "", "true");
    parser.add_option<bool>("cache_delta_results", "", "false");
    Heuristic::add_options_to_parser(parser);
}

bool
DeltaTrapHeuristic::supports_partial_state_evaluation() const
{
    return true;
}

bool
DeltaTrapHeuristic::add_conjunction_to_formula(const strips_utils::FactSet& fact_set)
{
#if TRAP_FORMULA_AS_UB_TREE
    bool result = conjunctions_.insert(fact_set, true);
#else
    bool result = conjunctions_.insert(fact_set).second;
#endif
    assert(is_satisfied(fact_set));
    return result;
}

bool
DeltaTrapHeuristic::is_satisfied(const strips_utils::FactSet& fact_set)
{
    return conjunctions_.contains_subset(fact_set);
}

void
DeltaTrapHeuristic::print_statistics() const
{
    std::cout << "**** Delta-Trap Heuristic ****" << std::endl;
    std::cout << "Formula size: " << conjunctions_.size() << std::endl;
}

int
DeltaTrapHeuristic::compute_heuristic(const GlobalState& state)
{
    strips_utils::FactSet facts = fact_map_->get_fact_set(state);
    return conjunctions_.contains_subset(facts) ? DEAD_END : 0;
}

int
DeltaTrapHeuristic::compute_heuristic(const PartialState& partial_state)
{
    strips_utils::FactSet facts;
    for (unsigned var = 0; var < g_variable_domain.size(); var++) {
        if (partial_state.is_defined(var)) {
            facts.push_back(fact_map_->get_fact(var, partial_state[var]));
        }
    }
    return conjunctions_.contains_subset(facts) ? DEAD_END : 0;
}

using AbstractStateRef = std::size_t;

class VariableSetIdMap {
public:
    explicit VariableSetIdMap(int n)
        : size_(0)
        , root_()
    {
        root_.initialize(g_variable_domain.size(), n, size_);
    }

    unsigned operator[](const std::vector<int>& variables) const
    {
        assert(std::is_sorted(variables.begin(), variables.end()));
        return root_.successors[variables[0]]->lookup(variables, 0);
    }

    unsigned size() const { return size_; }

private:
    struct Node {
        virtual ~Node() = default;
        virtual unsigned
        lookup(const std::vector<int>& vars, unsigned i) const = 0;
    };
    struct InnerNode : public Node {
        explicit InnerNode()
            : successors(nullptr)
        {
        }
        ~InnerNode() { delete[](successors); }
        void initialize(unsigned num, int depth, unsigned& id);
        virtual unsigned
        lookup(const std::vector<int>& vars, unsigned i) const override
        {
            assert(i < vars.size());
            return successors[vars[i + 1] - vars[i] - 1]->lookup(vars, i + 1);
        }
        Node** successors;
    };
    struct LeafNode : public Node {
        explicit LeafNode(unsigned id)
            : id(id)
        {
        }
        virtual unsigned
        lookup(const std::vector<int>&, unsigned) const override
        {
            return id;
        }
        unsigned id;
    };
    unsigned size_;
    InnerNode root_;
};

static void
initialize_varset_hasher(
    const VariableSetIdMap& varset_map,
    unsigned i,
    int var,
    std::vector<int>& varset,
    std::vector<perfect_hashing::PerfectHashingFunction>& hashers,
    std::vector<std::vector<int>>& varsets)
{
    if (i == varset.size()) {
#ifndef NDEBUG
        assert(hashers.size() < varset_map.size());
        assert(varset_map[varset] == hashers.size());
#endif
        varsets.push_back(varset);
        hashers.emplace_back(varset, g_variable_domain);
    } else {
        for (; var < static_cast<int>(
                   g_variable_domain.size() - varset.size() + i + 1);
             var++) {
            varset[i] = var;
            initialize_varset_hasher(
                varset_map, i + 1, var + 1, varset, hashers, varsets);
        }
    }
}

struct OperatorInfo {
    OperatorInfo()
        : postcondition(g_variable_domain.size(), -1)
    {
    }
    void initialize(
        const GlobalOperator& op,
        std::vector<std::pair<int, int>>& precond)
    {
        {
            const auto& pres = op.get_preconditions();
            for (int i = pres.size() - 1; i >= 0; --i) {
                postcondition[pres[i].var] = pres[i].val;
                precond.emplace_back(pres[i].var, pres[i].val);
                post_variables.push_back(pres[i].var);
            }
            std::sort(precond.begin(), precond.end());
        }
        {
            const auto& effs = op.get_effects();
            effect_variables.reserve(effs.size());
            for (int i = effs.size() - 1; i >= 0; --i) {
                if (postcondition[effs[i].var] == -1) {
                    post_variables.push_back(effs[i].var);
                }
                postcondition[effs[i].var] = effs[i].val;
                effect_variables.push_back(effs[i].var);
            }
            std::sort(effect_variables.begin(), effect_variables.end());
        }
        std::sort(post_variables.begin(), post_variables.end());
    }
    std::vector<int> postcondition;
    std::vector<int> effect_variables;
    std::vector<int> post_variables;

    ::utils::HashMap<std::vector<std::pair<int, int>>, bool> delta_result;
};

struct AbstractStateInfo {
    AbstractStateInfo()
        : status(NEW)
    {
    }

    bool is_new() const { return status == NEW; }
    bool is_open() const { return OPEN & status; }
    bool is_onstack() const { return STACK & status; }
    bool is_closed() const { return CLOSED & status; }
    bool is_alive() const { return GOAL & status; }
    bool is_dead() const { return is_closed() && !is_alive(); }

    void open(bool goal) { status = goal ? (OPEN | GOAL) : OPEN; }
    void set_onstack() { status = (status & GOAL) | STACK; }
    void close() { status = (status & GOAL) | CLOSED; }
    void set_alive() { status = status | GOAL; }

private:
    static const unsigned NEW = 0;
    static const unsigned GOAL = 1;
    static const unsigned OPEN = 2;
    static const unsigned STACK = 4;
    static const unsigned CLOSED = 8;
    unsigned status;
};

struct ExpansionInfo {
    ExpansionInfo(int varset_id, const AbstractStateRef& state)
        : varset_id(varset_id)
        , state(state)
        , succi(0)
        , goal(false)
        , self_loop(false)
    {
    }
    ExpansionInfo(ExpansionInfo&&) = default;

    void pop_successor()
    {
        assert(succi >= 0);
        if (succi + 1 != static_cast<int>(successors.size())) {
            successors[succi] = successors.back();
        }
        successors.pop_back();
    }

    int varset_id;
    AbstractStateRef state;
    std::vector<OperatorInfo*> aops;
    std::vector<std::pair<unsigned, AbstractStateRef>> successors;
    int succi;
    bool goal;
    bool self_loop;
};

struct StackStateInfo {
    StackStateInfo()
        : state(-1)
        , index(-1)
        , lowlink(-1)
    {
    } // allow resize ...

    explicit StackStateInfo(const AbstractStateRef& state)
        : state(state)
        , index(-1)
        , lowlink(-1)
    {
    }

    bool operator==(const AbstractStateRef& other_state) const
    {
        return state == other_state;
    }

    AbstractStateRef state;
    unsigned index;
    unsigned lowlink;
    int stack_idx;
    std::vector<int> transitions;
    std::vector<std::pair<int, int>> head_refs;
};

class AbstractStateEnumerator {
public:
    explicit AbstractStateEnumerator(
        int k,
        const VariableSetIdMap& varset_id_map,
        const std::vector<perfect_hashing::PerfectHashingFunction>& state_hashing);

    template<typename F, typename V>
    void enumerate_states(
        F& f,
        const std::vector<int>& variables0,
        const std::vector<int>& variables1,
        V& values);

    template<typename F>
    void enumerate_states(F& f, std::vector<int>& values);

    // template<typename F>
    // void enumerate_states(
    //    F& f,
    //    const std::vector<int>& variables,
    //    const std::vector<std::pair<int, int>>& values);

    // template<typename F>
    // void enumerate_states(F& f, const std::vector<std::pair<int, int>>&
    // values);

    // void enumerate_states(
    //    std::vector<std::pair<int, int>>& values,
    //    std::deque<AbstractStateRef>& result);

    // void enumerate_states(
    //    const std::vector<int>& vars,
    //    std::vector<std::pair<int, int>>& values,
    //    std::deque<AbstractStateRef>& result);

private:
    void load_operator(const GlobalOperator& op);

    void enumerate_hyperedges();

    template<typename F, typename V>
    void enumerate_var_sets(
        V& values,
        F& f,
        int n,
        std::vector<int>& varset,
        const std::vector<int>& a,
        unsigned i,
        const std::vector<int>& b,
        unsigned j);

    template<typename F, typename V>
    void enumerate_var_sets(
        V& values,
        F& f,
        int n,
        std::vector<int>& varset,
        const std::vector<int>& a,
        unsigned i);

    template<typename F>
    void enumerate_var_sets(F& f, int n, int var, std::vector<int>& varset);

    template<typename F, typename V>
    void enumerate_states(
        V& v,
        F& f,
        const perfect_hashing::PerfectHashingFunction& hasher,
        const std::vector<int>& variables,
        unsigned i);

    const unsigned k_;
    const VariableSetIdMap& varset_id_;
    const std::vector<perfect_hashing::PerfectHashingFunction>& state_hash_;

    std::vector<int> varset_;
    std::vector<int>* values_;
};

void
DeltaTrapHeuristic::build_k_trap(
    int k,
    const std::vector<std::shared_ptr<Heuristic>>& deltas)
{
    std::cout << "Building " << k << "-trap ..." << std::endl;
    ::utils::Timer timer;

    VariableSetIdMap varset_ids(k);
    std::vector<perfect_hashing::PerfectHashingFunction> varset_state_hasher;
    std::vector<std::vector<int>> varsets;
    varset_state_hasher.reserve(varset_ids.size());
    varsets.reserve(varset_ids.size());
    {
        std::vector<int> vars(k, 0);
        initialize_varset_hasher(
            varset_ids, 0, 0, vars, varset_state_hasher, varsets);
    }

    assert(varset_ids.size() == varset_state_hasher.size());

    unsigned num_abstract_states = 0;
    for (unsigned i = 0; i < varset_state_hasher.size(); i++) {
        assert(varset_state_hasher[i].is_within_limits());
        const unsigned offset = num_abstract_states;
        num_abstract_states += varset_state_hasher[i].get_max_hash();
        varset_state_hasher[i].set_offset(offset);
        assert(varset_state_hasher[i].is_within_limits());
    }

    std::vector<OperatorInfo> operators(g_operators.size());
    std::unique_ptr<successor_generator::SuccessorGenerator<OperatorInfo*>>
        aops_generator = nullptr;
    {
        std::vector<OperatorInfo*> opptrs;
        std::vector<std::vector<std::pair<int, int>>> preconditions;
        opptrs.reserve(operators.size());
        preconditions.reserve(operators.size());
        for (unsigned i = 0; i < g_operators.size(); ++i) {
            preconditions.emplace_back();
            operators[i].initialize(g_operators[i], preconditions.back());
            if (operators[i].effect_variables.empty()) {
                preconditions.pop_back();
                continue;
            }
            opptrs.push_back(&operators[i]);
        }
        aops_generator = std::unique_ptr<
            successor_generator::SuccessorGenerator<OperatorInfo*>>(
            new successor_generator::SuccessorGenerator<OperatorInfo*>(
                g_variable_domain, preconditions, opptrs));
    }

    AbstractStateEnumerator state_enumerator(
        k, varset_ids, varset_state_hasher);

    std::vector<int> goal(g_variable_domain.size(), -1);
    for (int i = g_goal.size() - 1; i >= 0; --i) {
        goal[g_goal[i].first] = g_goal[i].second;
    }

    std::cout << "... initialized basic data structures [t=" << timer << "]"
              << std::endl;

    struct CollectStates {
        explicit CollectStates(const perfect_hashing::PerfectHashingFunction* first)
            : first(first)
        {
        }
        void operator()(
            const perfect_hashing::PerfectHashingFunction& h,
            const std::vector<int>&,
            const PartialState&,
            const AbstractStateRef state)
        {
            states.emplace_back(&h - first, state);
        }
        void operator()(
            const perfect_hashing::PerfectHashingFunction& h,
            const std::vector<int>&,
            const std::vector<int>&,
            const AbstractStateRef state)
        {
            states.emplace_back(&h - first, state);
        }
        const perfect_hashing::PerfectHashingFunction* first;
        std::vector<std::pair<unsigned, AbstractStateRef>> states;
    };
    CollectStates collect_states(&varset_state_hasher[0]);

    {
        std::vector<int> istate(g_initial_state_data);
        state_enumerator.enumerate_states(collect_states, istate);
    }

    std::vector<std::pair<unsigned, AbstractStateRef>> initial_states;
    collect_states.states.swap(initial_states);
    std::cout << "... starting hypergraph analysis from "
              << initial_states.size() << " initial conjunctions [t=" << timer
              << "]" << std::endl;

    unsigned initial_conjunctions = initial_states.size();
    unsigned reachable_conjunctions = 0;
    unsigned reachable_alive_conjunctions = 0;
    unsigned num_goal_conjunctions = 0;
    unsigned num_pruned_conjunctions = 0;
    unsigned num_transitions = 0;
    unsigned num_pruned_transitions = 0;
    unsigned num_pruned_transitions_by_delta = 0;
    unsigned num_self_loops = 0;

    PartialState abstract_pstate(g_variable_domain.size());
    std::deque<ExpansionInfo> queue;
    std::deque<StackStateInfo> stack;
    std::unordered_map<AbstractStateRef, StackStateInfo*> stack_state_infos;
    // std::unordered_map<AbstractStateRef, AbstractStateInfo> state_infos;
    segmented_vector::DynamicSegmentedVector<AbstractStateInfo> state_infos;
    unsigned index = 0;

    std::vector<std::pair<int, int>> partial_assignment(k);
    auto prepare_state_expansion = [&](const unsigned varset_id,
                                       const AbstractStateRef state,
                                       const std::vector<int>& varset,
                                       const std::vector<int>& values,
                                       AbstractStateInfo& state_info) {
        assert(state_info.is_new());
        assert(varset.size() == values.size());
        assert(varset.size() == partial_assignment.size());
        if (!deltas.empty()) {
            abstract_pstate.clear();
        }

        bool is_goal = true;
        for (int i = varset.size() - 1; i >= 0; i--) {
            const int var = varset[i];
            const int val = values[i];
            is_goal = is_goal && (goal[var] == -1 || goal[var] == val);
            partial_assignment[i].first = var;
            partial_assignment[i].second = val;
            abstract_pstate[var] = val;
        }
        state_info.open(is_goal);

        state_info.set_onstack();
        stack.emplace_back(state);
        StackStateInfo& stack_info = stack.back(); // stack_state_infos[state];
        stack_state_infos[state] = &stack_info;
        stack_info.index = stack_info.lowlink = index++;
        stack_info.stack_idx = stack.size() - 1;
        queue.emplace_back(varset_id, state);
        bool is_dead = false;
        if (!is_goal && delta_prune_states_ && !deltas.empty()) {
            for (unsigned i = 0; i < deltas.size(); ++i) {
                deltas[i]->evaluate(abstract_pstate);
                if (deltas[i]->is_dead_end()) {
                    is_dead = true;
                    break;
                }
            }
        }
        if (is_goal) {
            num_goal_conjunctions++;
        } else if (is_dead) {
            num_pruned_conjunctions++;
        } else {
            aops_generator->generate_applicable_ops(
                partial_assignment, queue.back().aops);
            num_transitions += queue.back().aops.size();
        }
    };

    std::vector<int> reset_vars;
    std::vector<int> state_values(k, -1);
    std::vector<int> varset_union;

    auto abstract_state_to_fact_set =
        [&varsets, &varset_state_hasher](
            const std::shared_ptr<strips_utils::STRIPS> fact_map,
            const AbstractStateRef& state) {
            auto it = std::upper_bound(
                varset_state_hasher.begin(),
                varset_state_hasher.end(),
                state,
                [](const AbstractStateRef& s, const auto& hasher) {
                    return s < hasher.get_offset();
                });
            const int varset_idx =
                std::distance(varset_state_hasher.begin(), it) - 1;
            const std::vector<int>& vars = varsets[varset_idx];
            std::vector<int> state_values(vars.size());
            varset_state_hasher[varset_idx].hash_to_values(
                state, vars, g_variable_domain, state_values);
            strips_utils::FactSet fact_set;
            for (unsigned i = 0; i < state_values.size(); i++) {
                fact_set.push_back(
                    fact_map->get_fact(vars[i], state_values[i]));
            }
            return fact_set;
        };

    while (!initial_states.empty()) {
        {
            std::pair<unsigned, AbstractStateRef> state = initial_states.back();
            initial_states.pop_back();
            AbstractStateInfo& state_info = state_infos[state.second];
            if (state_info.is_closed()) {
                continue;
            }
            reachable_conjunctions++;
            assert(state_info.is_new());
            varset_state_hasher[state.first].hash_to_values(
                state.second,
                varsets[state.first],
                g_variable_domain,
                state_values);
            prepare_state_expansion(
                state.first,
                state.second,
                varsets[state.first],
                state_values,
                state_info);
        }
        bool backtracked = false;
        bool bt_is_alive = false;
        bool bt_is_onstack = false;
        unsigned bt_lowlink = -1;
        while (!queue.empty()) {
            ExpansionInfo& e = queue.back();
            StackStateInfo& stack_info = *stack_state_infos[e.state];
            const std::vector<int>& vars = varsets[e.varset_id];
            if (backtracked) {
                if (bt_is_onstack) {
                    stack_info.lowlink =
                        std::min(stack_info.lowlink, bt_lowlink);
                } else {
                    e.goal = e.goal && bt_is_alive;
                    if (e.succi + 1 != static_cast<int>(e.successors.size())) {
                        e.successors[e.succi] = e.successors.back();
                    }
                    e.successors.pop_back();
                }
                backtracked = false;
                varset_state_hasher[e.varset_id].hash_to_values(
                    e.state, vars, g_variable_domain, state_values);
            }
            while (true) {
                if (--e.succi < 0) {
                    if (e.successors.empty()) {
                        if (e.goal && !e.self_loop) {
                            state_infos[e.state].set_alive();
                        }
                    } else {
                        if (e.goal && !e.self_loop) {
                            const int tref = stack_info.transitions.size();
                            stack_info.transitions.push_back(
                                e.successors.size());
                            for (int i = e.successors.size() - 1; i >= 0; --i) {
                                assert(
                                    stack_state_infos.find(
                                        e.successors[i].second)
                                    != stack_state_infos.end());
                                stack_state_infos[e.successors[i].second]
                                    ->head_refs.emplace_back(
                                        stack_info.stack_idx, tref);
                            }
                        }
                        e.successors.clear();
                    }
                    if (e.aops.empty()) {
                        backtracked = true;
                        break;
                    } else {
                        OperatorInfo* op = e.aops.back();
                        e.aops.pop_back();
                        e.self_loop = true;
                        for (unsigned i = 0; i < vars.size(); i++) {
                            e.self_loop = e.self_loop
                                && (state_values[i]
                                        == op->postcondition[vars[i]]
                                    || op->postcondition[vars[i]] == -1);
                            if (op->postcondition[vars[i]] == -1) {
                                op->postcondition[vars[i]] = state_values[i];
                                reset_vars.push_back(vars[i]);
                            }
                        }
                        if (e.self_loop) {
                            num_self_loops++;
                        }
                        e.goal = false;
                        if (!e.self_loop) {
                            if (reset_vars.empty()) {
                                state_enumerator.enumerate_states(
                                    collect_states,
                                    op->effect_variables,
                                    op->post_variables,
                                    op->postcondition);
                            } else {
                                varset_union = op->post_variables;
                                varset_union.insert(
                                    varset_union.end(),
                                    reset_vars.begin(),
                                    reset_vars.end());
                                std::inplace_merge(
                                    varset_union.begin(),
                                    varset_union.begin()
                                        + op->post_variables.size(),
                                    varset_union.end());
                                state_enumerator.enumerate_states(
                                    collect_states,
                                    op->effect_variables,
                                    varset_union,
                                    op->postcondition);
                                varset_union.clear();
                            }
                            e.successors.swap(collect_states.states);
                            e.goal = true;
                            if (prune_transitions_) {
                                for (int i = e.successors.size() - 1; i >= 0;
                                     --i) {
                                    if (state_infos[e.successors[i].second]
                                            .is_dead()) {
                                        e.goal = false;
                                        break;
                                    }
                                }
                            }
                            if (delta_prune_transitions_ && e.goal
                                && !deltas.empty()) {
                                const PartialState pstate(&op->postcondition);
                                if (cache_delta_estimates_) {
                                    std::vector<std::pair<int, int>> ctxt;
                                    if (!reset_vars.empty()) {
                                        unsigned i = 0;
                                        for (unsigned j = 0; j < vars.size();
                                             ++j) {
                                            if (vars[j] == reset_vars[i]) {
                                                ctxt.emplace_back(
                                                    vars[j], state_values[j]);
                                                if (++i == reset_vars.size()) {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    auto it = op->delta_result.find(ctxt);
                                    if (it == op->delta_result.end()) {
                                        for (unsigned i = 0; i < deltas.size();
                                             ++i) {
                                            deltas[i]->evaluate(pstate);
                                            if (deltas[i]->is_dead_end()) {
                                                e.goal = false;
                                                break;
                                            }
                                        }
                                        op->delta_result[ctxt] = e.goal;
                                    } else {
                                        if (!it->second) {
                                            e.goal = false;
                                        }
                                    }
                                } else {
                                    for (unsigned i = 0; i < deltas.size();
                                         ++i) {
                                        deltas[i]->evaluate(pstate);
                                        if (deltas[i]->is_dead_end()) {
                                            e.goal = false;
                                            break;
                                        }
                                    }
                                }
                                if (!e.goal) {
                                    num_pruned_transitions_by_delta++;
                                }
                            }
                        }
                        for (int i = reset_vars.size() - 1; i >= 0; --i) {
                            op->postcondition[reset_vars[i]] = -1;
                        }
                        reset_vars.clear();

                        if (!e.goal || e.successors.empty()) {
                            num_pruned_transitions++;
                            e.successors.clear();
                            continue;
                        }
                        e.succi = e.successors.size() - 1;
                    }
                }
                if (prune_transitions_) {
                    for (int i = e.succi; i >= 0; --i) {
                        if (state_infos[e.successors[i].second].is_dead()) {
                            e.goal = false;
                            break;
                        }
                    }
                    if (!e.goal) {
                        e.successors.clear();
                        e.succi = 0;
                        continue;
                    }
                }
                const std::pair<unsigned, AbstractStateRef>& succ =
                    e.successors[e.succi];
                if (succ.second == e.state) {
                    assert(e.self_loop);
                    e.pop_successor();
                    continue;
                }
                AbstractStateInfo& succ_info = state_infos[succ.second];
                assert(!succ_info.is_open());
                if (succ_info.is_new()) {
                    varset_state_hasher[succ.first].hash_to_values(
                        succ.second,
                        varsets[succ.first],
                        g_variable_domain,
                        state_values);
                    prepare_state_expansion(
                        succ.first,
                        succ.second,
                        varsets[succ.first],
                        state_values,
                        succ_info);
                    reachable_conjunctions++;
                    break;
                } else if (succ_info.is_onstack()) {
                    assert(
                        stack_state_infos.find(succ.second)
                        != stack_state_infos.end());
                    const StackStateInfo& succ_stack_info =
                        *stack_state_infos[succ.second];
                    stack_info.lowlink =
                        std::min(stack_info.lowlink, succ_stack_info.index);
                } else if (succ_info.is_closed()) {
                    e.goal = e.goal && succ_info.is_alive();
                    e.pop_successor();
                }
#if !defined(NDEBUG)
                else {
                    assert(false);
                }
#endif
            }
            if (backtracked) {
                bt_is_onstack = true;
                bt_lowlink = stack_info.lowlink;
                if (stack_info.index == stack_info.lowlink) {
                    int scc_size = 0;
                    auto rend = stack.rbegin();
                    while (true) {
                        scc_size++;
                        if (*(rend++) == e.state) {
                            break;
                        }
                    }
                    if (scc_size == 1) {
                        stack_state_infos.erase(
                            stack_state_infos.find(e.state));
                        AbstractStateInfo& info = state_infos[e.state];
                        info.close();
                        bt_is_alive = info.is_alive();
                        if (bt_is_alive) {
                            reachable_alive_conjunctions++;
                        } else {
                            add_conjunction_to_formula(
                                abstract_state_to_fact_set(fact_map_, e.state));
                        }
                    } else {
                        std::deque<int> propq;
                        std::vector<bool> is_alive(scc_size, false);
                        std::vector<std::vector<int>> counters(scc_size);
                        std::vector<std::vector<std::pair<int, int>>>
                            transitions(scc_size);
                        int idx = scc_size - 1;
                        const int offset = stack.size() - scc_size;
                        for (auto it = stack.rbegin(); it != rend; ++it) {
                            {
                                StackStateInfo& info = *it;
                                assert(
                                    static_cast<int>(info.stack_idx - offset)
                                    == idx);
                                assert(idx >= 0 && idx < scc_size);
                                counters[idx] = std::move(info.transitions);
                                for (int j = info.head_refs.size() - 1; j >= 0;
                                     --j) {
                                    info.head_refs[j].first -= offset;
                                    assert(
                                        info.head_refs[j].first >= 0
                                        && info.head_refs[j].first < scc_size);
                                }
                                transitions[idx] = std::move(info.head_refs);
                                stack_state_infos.erase(
                                    stack_state_infos.find(it->state));
                            }
                            {
                                AbstractStateInfo& info =
                                    state_infos[it->state];
                                info.close();
                                if (info.is_alive()) {
                                    is_alive[idx] = true;
                                    propq.push_back(idx);
                                }
                            }
                            --idx;
                        }
                        while (!propq.empty()) {
                            idx = propq.back();
                            assert(is_alive[idx]);
                            propq.pop_back();
                            const auto& trefs = transitions[idx];
                            for (int i = trefs.size() - 1; i >= 0; --i) {
                                const auto& t = trefs[i];
                                if (--counters[t.first][t.second] == 0
                                    && !is_alive[t.first]) {
                                    is_alive[t.first] = true;
                                    propq.push_back(t.first);
                                }
                            }
                        }
                        idx = is_alive.size() - 1;
                        for (auto it = stack.rbegin(); it != rend; ++it) {
                            if (is_alive[idx--]) {
                                state_infos[it->state].set_alive();
                                reachable_alive_conjunctions++;
                            } else {
                                add_conjunction_to_formula(
                                    abstract_state_to_fact_set(
                                        fact_map_, it->state));
                            }
                        }
                        bt_is_alive = is_alive.front();
                    }
                    stack.resize(stack.size() - scc_size);
                    bt_is_onstack = false;
                }
                queue.pop_back();
            }
        }
        if (!bt_is_alive) {
            std::cout << "... initial state is dead-end!" << std::endl;
            break;
        }
    }

    assert(
        conjunctions_.size()
        == (reachable_conjunctions - reachable_alive_conjunctions));

    std::cout << "... completed hypergraph traversal [t=" << timer << "]"
              << std::endl;
    std::cout << "... generated " << num_transitions << " transitions (thereof "
              << num_self_loops << " self loops, "
              << num_pruned_transitions_by_delta << " pruned by Delta, and "
              << num_pruned_transitions << " pruned in total)" << std::endl;
    std::cout << "... " << reachable_conjunctions << " of "
              << num_abstract_states << " conjunctions are reachable (thereof "
              << initial_conjunctions << " true initially, "
              << num_goal_conjunctions << " don't contradict the goal)"
              << std::endl;
    std::cout << "... " << num_pruned_conjunctions
              << " conjunctions were recognized as dead-end by Delta"
              << std::endl;
    std::cout << "... "
              << (reachable_conjunctions - reachable_alive_conjunctions)
              << " conjunctions are dead" << std::endl;
    std::cout << "... finished Delta-trap initialization with "
              << conjunctions_.size() << " conjunctions [t=" << timer << "]"
              << std::endl;
}

void
VariableSetIdMap::InnerNode::initialize(unsigned num, int depth, unsigned& id)
{
    if (depth == 1) {
        successors = new Node*[num];
        for (unsigned i = 0; i < num; i++) {
            successors[i] = new LeafNode(id++);
        }
    } else {
        successors = new Node*[num - depth + 1];
        for (unsigned i = 0; i < num - depth + 1; i++) {
            InnerNode* succ = new InnerNode();
            succ->initialize(num - i - 1, depth - 1, id);
            successors[i] = succ;
        }
    }
}

AbstractStateEnumerator::AbstractStateEnumerator(
    int k,
    const VariableSetIdMap& varset_id_map,
    const std::vector<perfect_hashing::PerfectHashingFunction>& state_hashing)
    : k_(k)
    , varset_id_(varset_id_map)
    , state_hash_(state_hashing)
{
}

template<typename F>
void
AbstractStateEnumerator::enumerate_states(F& f, std::vector<int>& values)
{
    values_ = &values;
    enumerate_var_sets(f, k_, 0, varset_);
}

template<typename F, typename V>
void
AbstractStateEnumerator::enumerate_states(
    F& f,
    const std::vector<int>& must_variables,
    const std::vector<int>& may_variables,
    V& values)
{
    enumerate_var_sets(
        values, f, k_, varset_, must_variables, 0, may_variables, 0);
}

template<typename F, typename V>
void
AbstractStateEnumerator::enumerate_var_sets(
    V& v,
    F& f,
    int n,
    std::vector<int>& varset,
    const std::vector<int>& must_variables,
    unsigned i,
    const std::vector<int>& may_variables,
    unsigned j)
{
    assert(n > 0);
    assert(i < must_variables.size());
    assert(varset.empty() || varset.back() < must_variables[i]);
    if (n == 1) {
        for (; i < must_variables.size(); ++i) {
            varset.push_back(must_variables[i]);
            enumerate_states(v, f, state_hash_[varset_id_[varset]], varset, 0);
            varset.pop_back();
        }
    } else {
        for (; j < may_variables.size() - n + 1; ++j) {
            while (i < must_variables.size()
                   && must_variables[i] < may_variables[j]) {
                i++;
            }
            if (i == must_variables.size()) {
                break;
            }
            varset.push_back(may_variables[j]);
            if (must_variables[i] == may_variables[j]) {
                enumerate_var_sets(v, f, n - 1, varset, may_variables, j + 1);
            } else {
                enumerate_var_sets(
                    v,
                    f,
                    n - 1,
                    varset,
                    must_variables,
                    i,
                    may_variables,
                    j + 1);
            }
            varset.pop_back();
        }
    }
}

template<typename F, typename V>
void
AbstractStateEnumerator::enumerate_var_sets(
    V& v,
    F& f,
    int n,
    std::vector<int>& varset,
    const std::vector<int>& may_variables,
    unsigned j)
{
    assert(n > 0);
    assert(j < may_variables.size());
    if (n == 1) {
        for (; j < may_variables.size(); ++j) {
            varset.push_back(may_variables[j]);
            enumerate_states(v, f, state_hash_[varset_id_[varset]], varset, 0);
            varset.pop_back();
        }
    } else {
        for (; j < may_variables.size() - n + 1; ++j) {
            varset.push_back(may_variables[j]);
            enumerate_var_sets(v, f, n - 1, varset, may_variables, j + 1);
            varset.pop_back();
        }
    }
}

template<typename F>
void
AbstractStateEnumerator::enumerate_var_sets(
    F& f,
    int n,
    int var,
    std::vector<int>& varset)
{
    assert(n > 0);
    if (n == 1) {
        for (; var < static_cast<int>(g_variable_domain.size()); ++var) {
            varset.push_back(var);
            enumerate_states(
                *values_, f, state_hash_[varset_id_[varset]], varset, 0);
            varset.pop_back();
        }
    } else {
        for (; var < static_cast<int>(g_variable_domain.size()) - n + 1;
             ++var) {
            varset.push_back(var);
            enumerate_var_sets(f, n - 1, var + 1, varset);
            varset.pop_back();
        }
    }
}

template<typename F, typename V>
void
AbstractStateEnumerator::enumerate_states(
    V& values,
    F& f,
    const perfect_hashing::PerfectHashingFunction& hasher,
    const std::vector<int>& variables,
    unsigned i)
{
    if (i == variables.size()) {
        const AbstractStateRef tail = hasher.hash(variables, values);
        f(hasher, variables, values, tail);
    } else {
        const int var = variables[i];
        if (values[var] == -1) {
            for (int val = 0; val < g_variable_domain[var]; ++val) {
                values[var] = val;
                enumerate_states(values, f, hasher, variables, i + 1);
            }
            values[var] = -1;
        } else {
            enumerate_states(values, f, hasher, variables, i + 1);
        }
    }
}

} // namespace traps
