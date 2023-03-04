#include "probfd/tasks/root_task.h"

#include "probfd/value_type.h"

#include "option_parser.h"
#include "plugin.h"
#include "state_registry.h"

#include "utils/collections.h"
#include "utils/timer.h"

#include <algorithm>
#include <cassert>
#include <istream>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

using namespace std;
using utils::ExitCode;

namespace probfd {
namespace tasks {

static const auto PRE_FILE_PROB_VERSION = "3P";
shared_ptr<ProbabilisticTask> g_root_task = nullptr;

struct ExplicitVariable {
    int domain_size;
    string name;
    vector<string> fact_names;
    int axiom_layer;
    int axiom_default_value;

    explicit ExplicitVariable(std::istream& in);
};

struct ExplicitEffect {
    FactPair fact;
    vector<FactPair> conditions;

    ExplicitEffect(int var, int value, vector<FactPair>&& conditions);
};

struct ProbabilisticOutcome {
    value_t probability;
    vector<ExplicitEffect> effects;

    explicit ProbabilisticOutcome(std::istream& in);

    void read_pre_post(std::istream& in);
};

struct ExplicitOperator {
    vector<FactPair> preconditions;
    vector<ProbabilisticOutcome> outcomes;
    int cost;
    string name;
    int outcomes_start_index;

    ExplicitOperator(
        std::istream& in,
        bool use_metric,
        int outcomes_start_index);
};

struct ExplicitAxiom {
    vector<FactPair> preconditions;
    vector<ExplicitEffect> effects;
    string name;

    explicit ExplicitAxiom(std::istream& in);

    void read_pre_post(std::istream& in);
};

class UnusedRootTask : public ProbabilisticTask {
    vector<ExplicitVariable> variables;
    // TODO: think about using hash sets here.
    vector<vector<set<FactPair>>> mutexes;
    vector<ExplicitOperator> operators;
    vector<ExplicitAxiom> axioms;
    vector<int> initial_state_values;
    vector<FactPair> goals;

    const ExplicitVariable& get_variable(int var) const;
    const ExplicitAxiom& get_axiom(int index) const;
    const ExplicitOperator& get_operator(int index) const;
    const ProbabilisticOutcome& get_outcome(int op_id, int outcome_index) const;
    const ExplicitEffect& get_axiom_effect_(int op_id, int effect_id) const;
    const ExplicitEffect&
    get_operator_effect(int op_id, int outcome_index, int effect_id) const;

public:
    explicit UnusedRootTask(std::istream& in);

    int get_num_variables() const override;
    string get_variable_name(int var) const override;
    int get_variable_domain_size(int var) const override;
    int get_variable_axiom_layer(int var) const override;
    int get_variable_default_axiom_value(int var) const override;
    string get_fact_name(const FactPair& fact) const override;
    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    int get_num_axioms() const override;
    std::string get_axiom_name(int index) const override;
    int get_num_axiom_preconditions(int index) const override;
    FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    int get_num_axiom_effects(int op_index) const override;
    int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const override;
    FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
    FactPair get_axiom_effect(int op_index, int eff_index) const override;

    int get_num_operators() const override;
    int get_operator_cost(int index) const override;
    string get_operator_name(int index) const override;

    int get_num_operator_preconditions(int index) const override;
    FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    int get_num_operator_outcomes(int index) const override;

    value_t get_operator_outcome_probability(int index, int outcome_index)
        const override;

    int get_operator_outcome_id(int index, int outcome_index) const override;

    int get_num_operator_outcome_effects(int op_index, int outcome_index)
        const override;
    FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const override;

    int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const override;
    FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const override;

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;

    vector<int> get_initial_state_values() const override;

    void convert_ancestor_state_values(
        vector<int>& values,
        const AbstractTaskBase* ancestor_task) const override;

    int convert_operator_index(int index, const AbstractTaskBase* ancestor_task)
        const override;
};

static void
check_fact(const FactPair& fact, const vector<ExplicitVariable>& variables)
{
    if (!utils::in_bounds(fact.var, variables)) {
        cerr << "Invalid variable id: " << fact.var << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    if (fact.value < 0 || fact.value >= variables[fact.var].domain_size) {
        cerr << "Invalid value for variable " << fact.var << ": " << fact.value
             << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

static void check_facts(
    const vector<FactPair>& facts,
    const vector<ExplicitVariable>& variables)
{
    for (FactPair fact : facts) {
        check_fact(fact, variables);
    }
}

static void check_facts(
    const ExplicitAxiom& axiom,
    const vector<ExplicitVariable>& variables)
{
    check_facts(axiom.preconditions, variables);

    for (const ExplicitEffect& eff : axiom.effects) {
        check_fact(eff.fact, variables);
        check_facts(eff.conditions, variables);
    }
}

static void check_facts(
    const ExplicitOperator& action,
    const vector<ExplicitVariable>& variables)
{
    check_facts(action.preconditions, variables);

    value_t total_prob = 0_vt;

    for (const auto& outcome : action.outcomes) {
        const auto prob = outcome.probability;

        if (prob <= 0_vt) {
            cerr << "Probability must be greater than zero: " << prob << endl;
            utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
        } else if (prob > 1_vt) {
            cerr << "Probability must be less or equal to one: " << prob
                 << endl;
            utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
        }

        for (const ExplicitEffect& eff : outcome.effects) {
            check_fact(eff.fact, variables);
            check_facts(eff.conditions, variables);
        }

        total_prob += prob;
    }

    if (is_approx_equal(total_prob, 1_vt)) {
        cerr << "Total outcome probabilities must sum up to one. Sum was: "
             << total_prob << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

void check_magic(std::istream& in, const string& magic)
{
    string word;
    in >> word;
    if (word != magic) {
        cerr << "Failed to match magic word '" << magic << "'." << endl
             << "Got '" << word << "'." << endl;
        if (magic == "begin_version") {
            cerr << "Possible cause: you are running the planner "
                 << "on a translator output file from " << endl
                 << "an older version." << endl;
        }
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

vector<FactPair> read_facts(std::istream& in)
{
    int count;
    in >> count;
    vector<FactPair> conditions;
    conditions.reserve(count);
    for (int i = 0; i < count; ++i) {
        FactPair condition = FactPair::no_fact;
        in >> condition.var >> condition.value;
        conditions.push_back(condition);
    }
    return conditions;
}

ExplicitVariable::ExplicitVariable(std::istream& in)
{
    check_magic(in, "begin_variable");
    in >> name;
    in >> axiom_layer;
    in >> domain_size;
    in >> ws;
    fact_names.resize(domain_size);
    for (int i = 0; i < domain_size; ++i)
        getline(in, fact_names[i]);
    check_magic(in, "end_variable");
}

ExplicitEffect::ExplicitEffect(
    int var,
    int value,
    vector<FactPair>&& conditions)
    : fact(var, value)
    , conditions(move(conditions))
{
}

ProbabilisticOutcome::ProbabilisticOutcome(std::istream& in)
{
    // Read number of effects
    int count;
    in >> count;
    effects.reserve(count);

    // Read each effect
    for (int i = 0; i < count; ++i) {
        read_pre_post(in);
    }
}

void ProbabilisticOutcome::read_pre_post(std::istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, value_post;
    in >> var >> value_post;
    effects.emplace_back(var, value_post, move(conditions));
}

ExplicitOperator::ExplicitOperator(
    std::istream& in,
    bool use_metric,
    int outcomes_start_index)
    : outcomes_start_index(outcomes_start_index)
{
    check_magic(in, "begin_operator");
    in >> ws;
    getline(in, name);
    preconditions = read_facts(in);
    int count;
    in >> count;
    outcomes.reserve(count);
    for (int i = 0; i < count; ++i) {
        outcomes.emplace_back(in);
    }

    int op_cost;
    in >> op_cost;
    cost = use_metric ? op_cost : 1;
    check_magic(in, "end_operator");

    assert(cost >= 0);
}

ExplicitAxiom::ExplicitAxiom(std::istream& in)
{
    name = "<axiom>";
    check_magic(in, "begin_rule");
    int count;
    in >> count;
    effects.reserve(count);
    for (int i = 0; i < count; ++i) {
        read_pre_post(in);
    }
    check_magic(in, "end_rule");
}

void ExplicitAxiom::read_pre_post(std::istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, value_post;
    in >> var >> value_post;
    effects.emplace_back(var, value_post, move(conditions));
}

void read_and_verify_version(std::istream& in)
{
    std::string version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_PROB_VERSION) {
        cerr << "Expected translator output file version "
             << PRE_FILE_PROB_VERSION << ", got " << version << "." << endl
             << "Exiting." << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

bool read_metric(std::istream& in)
{
    bool use_metric;
    check_magic(in, "begin_metric");
    in >> use_metric;
    check_magic(in, "end_metric");
    return use_metric;
}

vector<ExplicitVariable> read_variables(std::istream& in)
{
    int count;
    in >> count;
    vector<ExplicitVariable> variables;
    variables.reserve(count);
    for (int i = 0; i < count; ++i) {
        variables.emplace_back(in);
    }
    return variables;
}

vector<vector<set<FactPair>>>
read_mutexes(std::istream& in, const vector<ExplicitVariable>& variables)
{
    vector<vector<set<FactPair>>> inconsistent_facts(variables.size());
    for (size_t i = 0; i < variables.size(); ++i)
        inconsistent_facts[i].resize(variables[i].domain_size);

    int num_mutex_groups;
    in >> num_mutex_groups;

    /*
      NOTE: Mutex groups can overlap, in which case the same mutex
      should not be represented multiple times. The current
      representation takes care of that automatically by using sets.
      If we ever change this representation, this is something to be
      aware of.
    */
    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        vector<FactPair> invariant_group;
        invariant_group.reserve(num_facts);
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
            invariant_group.emplace_back(var, value);
        }
        check_magic(in, "end_mutex_group");
        for (const FactPair& fact1 : invariant_group) {
            for (const FactPair& fact2 : invariant_group) {
                if (fact1.var != fact2.var) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the translator (at least
                       with default settings) removes mutex groups
                       that contain *only* redundant mutexes, but it
                       can of course generate mutex groups which lead
                       to *some* redundant mutexes, where some but not
                       all facts talk about the same variable. */
                    inconsistent_facts[fact1.var][fact1.value].insert(fact2);
                }
            }
        }
    }
    return inconsistent_facts;
}

vector<FactPair> read_goal(std::istream& in)
{
    check_magic(in, "begin_goal");
    vector<FactPair> goals = read_facts(in);
    check_magic(in, "end_goal");
    if (goals.empty()) {
        cerr << "Task has no goal condition!" << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
    return goals;
}

vector<ExplicitAxiom>
read_axioms(std::istream& in, const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitAxiom> axioms;
    axioms.reserve(count);
    for (int i = 0; i < count; ++i) {
        axioms.emplace_back(in);
        check_facts(axioms.back(), variables);
    }
    return axioms;
}

vector<ExplicitOperator> read_operators(
    std::istream& in,
    bool use_metric,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitOperator> actions;
    actions.reserve(count);
    int outcomes_counter = 0;
    for (int i = 0; i < count; ++i) {
        const auto& op = actions.emplace_back(in, use_metric, outcomes_counter);
        outcomes_counter += op.outcomes.size();
        check_facts(actions.back(), variables);
    }
    return actions;
}

UnusedRootTask::UnusedRootTask(std::istream& in)
{
    read_and_verify_version(in);
    bool use_metric = read_metric(in);
    variables = read_variables(in);
    int num_variables = variables.size();

    mutexes = read_mutexes(in, variables);

    initial_state_values.resize(num_variables);
    check_magic(in, "begin_state");
    for (int i = 0; i < num_variables; ++i) {
        in >> initial_state_values[i];
    }
    check_magic(in, "end_state");

    for (int i = 0; i < num_variables; ++i) {
        variables[i].axiom_default_value = initial_state_values[i];
    }

    goals = read_goal(in);
    check_facts(goals, variables);
    operators = read_operators(in, use_metric, variables);
    axioms = read_axioms(in, variables);
    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    /*
      HACK: We use a TaskProxy to access g_axiom_evaluators here which assumes
      that this task is completely constructed.
    */
    AxiomEvaluator& axiom_evaluator = g_axiom_evaluators[TaskBaseProxy(*this)];
    axiom_evaluator.evaluate(initial_state_values);
}

const ExplicitVariable& UnusedRootTask::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables));
    return variables[var];
}

const ProbabilisticOutcome&
UnusedRootTask::get_outcome(int op_id, int outcome_index) const
{
    const ExplicitOperator& op = get_operator(op_id);
    assert(utils::in_bounds(outcome_index, op.outcomes));
    return op.outcomes[outcome_index];
}

const ExplicitEffect&
UnusedRootTask::get_axiom_effect_(int op_id, int effect_id) const
{
    const ExplicitAxiom& axiom = get_axiom(op_id);
    assert(utils::in_bounds(effect_id, axiom.effects));
    return axiom.effects[effect_id];
}

const ExplicitEffect&
UnusedRootTask::get_operator_effect(int op_id, int outcome_index, int effect_id)
    const
{
    const ProbabilisticOutcome& outcome = get_outcome(op_id, outcome_index);
    assert(utils::in_bounds(effect_id, outcome.effects));
    return outcome.effects[effect_id];
}

const ExplicitAxiom& UnusedRootTask::get_axiom(int index) const
{
    assert(utils::in_bounds(index, axioms));
    return axioms[index];
}

const ExplicitOperator& UnusedRootTask::get_operator(int index) const
{
    assert(utils::in_bounds(index, operators));
    return operators[index];
}

int UnusedRootTask::get_num_variables() const
{
    return variables.size();
}

string UnusedRootTask::get_variable_name(int var) const
{
    return get_variable(var).name;
}

int UnusedRootTask::get_variable_domain_size(int var) const
{
    return get_variable(var).domain_size;
}

int UnusedRootTask::get_variable_axiom_layer(int var) const
{
    return get_variable(var).axiom_layer;
}

int UnusedRootTask::get_variable_default_axiom_value(int var) const
{
    return get_variable(var).axiom_default_value;
}

string UnusedRootTask::get_fact_name(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
    return get_variable(fact.var).fact_names[fact.value];
}

bool UnusedRootTask::are_facts_mutex(
    const FactPair& fact1,
    const FactPair& fact2) const
{
    if (fact1.var == fact2.var) {
        // Same variable: mutex iff different value.
        return fact1.value != fact2.value;
    }
    assert(utils::in_bounds(fact1.var, mutexes));
    assert(utils::in_bounds(fact1.value, mutexes[fact1.var]));
    return bool(mutexes[fact1.var][fact1.value].count(fact2));
}

int UnusedRootTask::get_num_axioms() const
{
    return axioms.size();
}

string UnusedRootTask::get_axiom_name(int index) const
{
    return get_axiom(index).name;
}

int UnusedRootTask::get_num_axiom_preconditions(int index) const
{
    return get_axiom(index).preconditions.size();
}

FactPair
UnusedRootTask::get_axiom_precondition(int op_index, int fact_index) const
{
    const ExplicitAxiom& axiom = get_axiom(op_index);
    assert(utils::in_bounds(fact_index, axiom.preconditions));
    return axiom.preconditions[fact_index];
}

int UnusedRootTask::get_num_axiom_effects(int op_index) const
{
    return get_axiom(op_index).effects.size();
}

FactPair UnusedRootTask::get_axiom_effect(int op_index, int eff_index) const
{
    return get_axiom_effect_(op_index, eff_index).fact;
}

int UnusedRootTask::get_num_axiom_effect_conditions(int op_index, int eff_index)
    const
{
    return get_axiom_effect_(op_index, eff_index).conditions.size();
}

FactPair UnusedRootTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect = get_axiom_effect_(op_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

int UnusedRootTask::get_num_operators() const
{
    return operators.size();
}

int UnusedRootTask::get_operator_cost(int index) const
{
    return get_operator(index).cost;
}

string UnusedRootTask::get_operator_name(int index) const
{
    return get_operator(index).name;
}

int UnusedRootTask::get_num_operator_preconditions(int index) const
{
    return get_operator(index).preconditions.size();
}

FactPair
UnusedRootTask::get_operator_precondition(int op_index, int fact_index) const
{
    const ExplicitOperator& op = get_operator(op_index);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int UnusedRootTask::get_num_operator_outcomes(int index) const
{
    return get_operator(index).outcomes.size();
}

value_t
UnusedRootTask::get_operator_outcome_probability(int index, int outcome_index)
    const
{
    return get_outcome(index, outcome_index).probability;
}

int UnusedRootTask::get_operator_outcome_id(int index, int outcome_index) const
{
    return get_operator(index).outcomes_start_index + outcome_index;
}

int UnusedRootTask::get_num_operator_outcome_effects(
    int op_index,
    int outcome_index) const
{
    return get_outcome(op_index, outcome_index).effects.size();
}

FactPair UnusedRootTask::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index).fact;
}

int UnusedRootTask::get_num_operator_outcome_effect_conditions(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index)
        .conditions.size();
}

FactPair UnusedRootTask::get_operator_outcome_effect_condition(
    int op_index,
    int outcome_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect =
        get_operator_effect(op_index, outcome_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

int UnusedRootTask::get_num_goals() const
{
    return goals.size();
}

FactPair UnusedRootTask::get_goal_fact(int index) const
{
    assert(utils::in_bounds(index, goals));
    return goals[index];
}

vector<int> UnusedRootTask::get_initial_state_values() const
{
    return initial_state_values;
}

void UnusedRootTask::convert_ancestor_state_values(
    vector<int>&,
    const AbstractTaskBase* ancestor_task) const
{
    if (this != ancestor_task) {
        ABORT("Invalid state conversion");
    }
}

int UnusedRootTask::convert_operator_index(
    int index,
    const AbstractTaskBase* ancestor_task) const
{
    if (this != ancestor_task) {
        ABORT("Invalid operator ID conversion");
    }
    return index;
}

} // namespace tasks
} // namespace probfd