#include "globals.h"

#include "algorithms/int_packer.h"

#include "heuristics/domain_transition_graph.h"

#include "axioms.h"
#include "global_operator.h"
#include "global_state.h"
#include "heuristic.h"

#include "task_utils/causal_graph.h"
#include "task_utils/successor_generator.h"

#include "utils/rng.h"
#include "utils/system.h"
#include "utils/timer.h"

#include "probfd/globals.h"
#include "probfd/probabilistic_operator.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <set>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

static const string PRE_FILE_VERSION = "3";
static const string PROB_INDICATOR_VERSION = "3P";

// TODO: This needs a proper type and should be moved to a separate
//       mutexes.cc file or similar, accessed via something called
//       g_mutexes. (Right now, the interface is via global function
//       are_mutex, which is at least better than exposing the data
//       structure globally.)

bool test_goal(const GlobalState &state) {
    for (size_t i = 0; i < g_goal.size(); ++i) {
        if (state[g_goal[i].first] != g_goal[i].second) {
            return false;
        }
    }
    return true;
}

bool peek_magic(istream &in, string magic) {
    string word;
    in >> word;
    bool result = (word == magic);
    for (int i = word.size() - 1; i >= 0; --i)
        in.putback(word[i]);
    return result;
}

void check_magic(istream &in, string magic) {
    string word;
    in >> word;
    if (word != magic) {
        cout << "Failed to match magic word '" << magic << "'." << endl;
        cout << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the planner "
                 << "on a preprocessor file from " << endl
                 << "an older version." << endl;
        }
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

bool read_and_verify_version(istream &in) {
    string version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_VERSION && version != PROB_INDICATOR_VERSION) {
        cerr << "Expected preprocessor file version " << PRE_FILE_VERSION
             << ", got " << version << "." << endl;
        cerr << "Exiting." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    return version == PROB_INDICATOR_VERSION;
}

void read_metric(istream &in) {
    check_magic(in, "begin_metric");
    in >> g_use_metric;
    check_magic(in, "end_metric");
}

void read_variables(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i) {
        check_magic(in, "begin_variable");
        string name;
        in >> name;
        g_variable_name.push_back(name);
        int layer;
        in >> layer;
        g_axiom_layers.push_back(layer);
        int range;
        in >> range;
        g_variable_domain.push_back(range);
        in >> ws;
        vector<string> fact_names(range);
        for (size_t j = 0; j < fact_names.size(); ++j)
            getline(in, fact_names[j]);
        g_fact_names.push_back(fact_names);
        check_magic(in, "end_variable");
    }
}

void read_mutexes(istream &in) {
    g_inconsistent_facts.resize(g_variable_domain.size());
    for (size_t i = 0; i < g_variable_domain.size(); ++i)
        g_inconsistent_facts[i].resize(g_variable_domain[i]);

    int num_mutex_groups;
    in >> num_mutex_groups;

    /* NOTE: Mutex groups can overlap, in which case the same mutex
       should not be represented multiple times. The current
       representation takes care of that automatically by using sets.
       If we ever change this representation, this is something to be
       aware of. */

    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        vector<pair<int, int> > invariant_group;
        invariant_group.reserve(num_facts);
        for (int j = 0; j < num_facts; ++j) {
            int var, val;
            in >> var >> val;
            invariant_group.push_back(make_pair(var, val));
        }
        check_magic(in, "end_mutex_group");
        for (size_t j = 0; j < invariant_group.size(); ++j) {
            const pair<int, int> &fact1 = invariant_group[j];
            int var1 = fact1.first, val1 = fact1.second;
            for (size_t k = 0; k < invariant_group.size(); ++k) {
                const pair<int, int> &fact2 = invariant_group[k];
                int var2 = fact2.first;
                if (var1 != var2) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the preprocessor removes
                       mutex groups that contain *only* redundant
                       mutexes, but it can of course generate mutex
                       groups which lead to *some* redundant mutexes,
                       where some but not all facts talk about the
                       same variable. */
                    g_inconsistent_facts[var1][val1].insert(fact2);
                }
            }
        }
    }
}

void read_goal(istream &in) {
    check_magic(in, "begin_goal");
    int count;
    in >> count;
    if (count < 1) {
        cerr << "Task has no goal condition!" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
    for (int i = 0; i < count; ++i) {
        int var, val;
        in >> var >> val;
        g_goal.push_back(make_pair(var, val));
    }
    check_magic(in, "end_goal");
}

void dump_initial_state() {
    cout << "Initial state:" << endl;
    for (size_t i = 0; i < g_initial_state_data.size(); ++i)
        cout << "  " << g_variable_name[i] << ": "
             << g_initial_state_data[i] << endl;
}

void dump_goal() {
    cout << "Goal Conditions:" << endl;
    for (size_t i = 0; i < g_goal.size(); ++i)
        cout << "  " << g_variable_name[g_goal[i].first] << ": "
             << g_goal[i].second << endl;
}

void read_operators(istream &in) {
    int count;
    in >> count;
    g_operators.reserve(count);
    for (int i = 0; i < count; ++i) {
        g_operators.push_back(GlobalOperator(in, false));
        g_operators.back().set_id(i);
    }
}

void read_axioms(istream &in) {
    int count;
    in >> count;
    for (int i = 0; i < count; ++i)
        g_axioms.push_back(GlobalOperator(in, true));

    g_axiom_evaluator = new AxiomEvaluator;
}

void read_probabilistic_operators(istream& in)
{
    int count;
    in >> count;
    for (int i = 0; i < count; ++i) {
        probfd::g_operators.emplace_back(i, in);
    }
}

void read_everything(istream &in) {
    cout << "reading input... [t=" << utils::g_timer << "]" << endl;
    bool is_prob = read_and_verify_version(in);
    read_metric(in);
    read_variables(in);
    read_mutexes(in);
    g_initial_state_data.resize(g_variable_domain.size());
    check_magic(in, "begin_state");
    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        in >> g_initial_state_data[i];
    }
    check_magic(in, "end_state");
    g_default_axiom_values = g_initial_state_data;

    read_goal(in);
    read_operators(in);
    read_axioms(in);
    if (is_prob) {
        read_probabilistic_operators(in);
    }

    // check_magic(in, "begin_SG");
    // g_successor_generator = read_successor_generator(in);
    // check_magic(in, "end_SG");
    // DomainTransitionGraph::read_all(in);
    // check_magic(in, "begin_CG"); // ignore everything from here

    cout << "done reading input! [t=" << utils::g_timer << "]" << endl;

    // NOTE: causal graph is computed from the problem specification,
    // so must be built after the problem has been read in.
 
    cout << "packing state variables..." << flush;
    assert(!g_variable_domain.empty());
    g_state_packer = new int_packer::IntPacker(g_variable_domain);
    cout << "done! [t=" << utils::g_timer << "]" << endl;

    // NOTE: state registry stores the sizes of the state, so must be
    // built after the problem has been read in.
    // g_state_registry = new StateRegistry;

    int num_vars = g_variable_domain.size();
    int num_facts = 0;
    for (int var = 0; var < num_vars; ++var)
        num_facts += g_variable_domain[var];

    cout << "Variables: " << num_vars << endl;
    cout << "Facts: " << num_facts << endl;
    cout << "Bytes per state: "
         << g_state_packer->get_num_bins() *
    g_state_packer->get_bin_size_in_bytes() << endl;

    cout << "done initalizing global data [t=" << utils::g_timer << "]" << endl;
}

void dump_everything() {
    cout << "Use metric? " << g_use_metric << endl;
    cout << "Min Action Cost: " << g_min_action_cost << endl;
    cout << "Max Action Cost: " << g_max_action_cost << endl;
    // TODO: Dump the actual fact names.
    cout << "Variables (" << g_variable_name.size() << "):" << endl;
    for (size_t i = 0; i < g_variable_name.size(); ++i)
        cout << "  " << g_variable_name[i]
             << " (range " << g_variable_domain[i] << ")" << endl;
    // GlobalState initial_state = g_initial_state();
    // cout << "Initial State (PDDL):" << endl;
    // initial_state.dump_pddl();
    // cout << "Initial State (FDR):" << endl;
    // initial_state.dump_fdr();
    dump_initial_state();
    dump_goal();
}

bool is_unit_cost() {
    return g_min_action_cost == 1 && g_max_action_cost == 1;
}

bool has_axioms() {
    return !g_axioms.empty();
}

void verify_no_axioms() {
    if (has_axioms()) {
        cerr << "Heuristic does not support axioms!" << endl << "Terminating."
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
    }
}

static int get_first_conditional_effects_op_id() {
    for (size_t i = 0; i < g_operators.size(); ++i) {
        const vector<GlobalEffect> &effects = g_operators[i].get_effects();
        for (size_t j = 0; j < effects.size(); ++j) {
            const vector<GlobalCondition> &cond = effects[j].conditions;
            if (!cond.empty())
                return i;
        }
    }
    return -1;
}

bool has_conditional_effects() {
    return get_first_conditional_effects_op_id() != -1;
}

void verify_no_conditional_effects() {
    int op_id = get_first_conditional_effects_op_id();
    if (op_id != -1) {
        cerr << "Heuristic does not support conditional effects "
             << "(operator " << g_operators[op_id].get_name() << ")" << endl
             << "Terminating." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
    }
}

void verify_no_axioms_no_conditional_effects() {
    verify_no_axioms();
    verify_no_conditional_effects();
}

bool are_mutex(const pair<int, int> &a, const pair<int, int> &b) {
    if (a.first == b.first) // same variable: mutex iff different value
        return a.second != b.second;
    return bool(g_inconsistent_facts[a.first][a.second].count(b));
}

static PackedStateBin* g_cached_initial_state = nullptr;
GlobalState g_initial_state()
{
    if (g_cached_initial_state == nullptr) {
        g_cached_initial_state = new PackedStateBin[g_state_packer->get_num_bins()];
        fill_n(g_cached_initial_state, g_state_packer->get_num_bins(), 0);
        for (size_t i = 0; i < g_initial_state_data.size(); ++i) {
            g_state_packer->set(g_cached_initial_state, i, g_initial_state_data[i]);
        }
    }
    return GlobalState(g_cached_initial_state, g_state_packer);
}

bool g_use_metric;
int g_min_action_cost = numeric_limits<int>::max();
int g_max_action_cost = 0;
vector<string> g_variable_name;
vector<int> g_variable_domain;
vector<vector<string> > g_fact_names;
vector<int> g_axiom_layers;
vector<int> g_default_axiom_values;
int_packer::IntPacker *g_state_packer;
vector<int> g_initial_state_data;
vector<pair<int, int> > g_goal;
vector<GlobalOperator> g_operators;
vector<GlobalOperator> g_axioms;
AxiomEvaluator *g_axiom_evaluator;
vector<DomainTransitionGraph *> g_transition_graphs;
causal_graph::CausalGraph *g_causal_graph;

string g_plan_filename = "sas_plan";
int g_num_previously_generated_plans = 0;
bool g_is_part_of_anytime_portfolio = false;

utils::RandomNumberGenerator g_rng(2011);
std::vector<std::vector<std::set<std::pair<int, int> > > > g_inconsistent_facts;
std::vector<std::vector<std::pair<int, int>>> g_dnf_formula;
