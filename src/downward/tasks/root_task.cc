#include "downward/tasks/root_task.h"

#include "downward/abstract_task.h"
#include "downward/state_registry.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

using namespace std;
using downward::utils::ExitCode;

namespace downward::tasks {
static constexpr auto PRE_FILE_VERSION = "3";
static constexpr auto PRE_FILE_PROB_VERSION = "3P";

struct ExplicitVariable {
    int domain_size;
    string name;
    vector<string> fact_names;
    int axiom_layer;
    int axiom_default_value;

    explicit ExplicitVariable(istream& in);
};

struct ExplicitEffect {
    FactPair fact;
    vector<FactPair> conditions;

    ExplicitEffect(int var, int value, vector<FactPair>&& conditions);
};

struct ExplicitOperator {
    vector<FactPair> preconditions;
    vector<ExplicitEffect> effects;
    int cost;
    string name;
    bool is_an_axiom;

    void read_pre_post(istream& in);
    ExplicitOperator(istream& in, bool is_an_axiom, bool use_metric);
};

class RootTask : public AbstractTask {
    vector<ExplicitVariable> variables_;
    vector<ExplicitOperator> operators_;
    vector<ExplicitOperator> axioms_;
    vector<int> initial_state_values_;
    vector<FactPair> goals_;

    const ExplicitVariable& get_variable(int var) const;
    const ExplicitEffect&
    get_effect(int op_id, int effect_id, bool is_axiom) const;
    const ExplicitOperator&
    get_operator_or_axiom(int index, bool is_axiom) const;

public:
    explicit RootTask(istream& in);

    int get_num_variables() const override;
    string get_variable_name(int var) const override;
    int get_variable_domain_size(int var) const override;
    int get_variable_axiom_layer(int var) const override;
    int get_variable_default_axiom_value(int var) const override;
    string get_fact_name(const FactPair& fact) const override;

    int get_num_axioms() const override;

    string get_axiom_name(int index) const override;
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
    int get_num_operator_effects(int op_index) const override;
    virtual int get_num_operator_effect_conditions(int op_index, int eff_index)
        const override;
    FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
    FactPair get_operator_effect(int op_index, int eff_index) const override;

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;

    vector<int> get_initial_state_values() const override;
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
    for (FactPair fact : facts) { check_fact(fact, variables); }
}

static void check_facts(
    const ExplicitOperator& action,
    const vector<ExplicitVariable>& variables)
{
    check_facts(action.preconditions, variables);
    for (const ExplicitEffect& eff : action.effects) {
        check_fact(eff.fact, variables);
        check_facts(eff.conditions, variables);
    }
}

static void check_magic(istream& in, const string& magic)
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

static vector<FactPair> read_facts(istream& in)
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

ExplicitVariable::ExplicitVariable(istream& in)
{
    check_magic(in, "begin_variable");
    in >> name;
    in >> axiom_layer;
    in >> domain_size;
    in >> ws;
    fact_names.resize(domain_size);
    for (int i = 0; i < domain_size; ++i) getline(in, fact_names[i]);
    check_magic(in, "end_variable");
}

ExplicitEffect::ExplicitEffect(
    int var,
    int value,
    vector<FactPair>&& conditions)
    : fact(var, value)
    , conditions(std::move(conditions))
{
}

void ExplicitOperator::read_pre_post(istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, value_pre, value_post;
    in >> var >> value_pre >> value_post;
    if (value_pre != -1) { preconditions.emplace_back(var, value_pre); }
    effects.emplace_back(var, value_post, std::move(conditions));
}

ExplicitOperator::ExplicitOperator(
    istream& in,
    bool is_an_axiom,
    bool use_metric)
    : is_an_axiom(is_an_axiom)
{
    if (!is_an_axiom) {
        check_magic(in, "begin_operator");
        in >> ws;
        getline(in, name);
        preconditions = read_facts(in);
        int count;
        in >> count;
        effects.reserve(count);
        for (int i = 0; i < count; ++i) { read_pre_post(in); }

        int op_cost;
        in >> op_cost;
        cost = use_metric ? op_cost : 1;
        check_magic(in, "end_operator");
    } else {
        name = "<axiom>";
        cost = 0;
        check_magic(in, "begin_rule");
        read_pre_post(in);
        check_magic(in, "end_rule");
    }
    assert(cost >= 0);
}

static void read_and_verify_version(istream& in)
{
    std::string version;
    check_magic(in, "begin_version");
    in >> version;
    check_magic(in, "end_version");
    if (version != PRE_FILE_VERSION && version != PRE_FILE_PROB_VERSION) {
        cerr << "Expected translator output file version " << PRE_FILE_VERSION
             << ", got " << version << "." << endl
             << "Exiting." << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }
}

static bool read_metric(istream& in)
{
    bool use_metric;
    check_magic(in, "begin_metric");
    in >> use_metric;
    check_magic(in, "end_metric");
    return use_metric;
}

static vector<ExplicitVariable> read_variables(istream& in)
{
    int count;
    in >> count;
    vector<ExplicitVariable> variables;
    variables.reserve(count);
    for (int i = 0; i < count; ++i) { variables.emplace_back(in); }
    return variables;
}

static vector<FactPair> read_goal(istream& in)
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

static vector<ExplicitOperator> read_actions(
    istream& in,
    bool is_axiom,
    bool use_metric,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitOperator> actions;
    actions.reserve(count);
    for (int i = 0; i < count; ++i) {
        actions.emplace_back(in, is_axiom, use_metric);
        check_facts(actions.back(), variables);
    }
    return actions;
}

static void skip_mutexes(std::istream& in)
{
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
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
        }
        check_magic(in, "end_mutex_group");
    }
}

RootTask::RootTask(istream& in)
{
    read_and_verify_version(in);
    bool use_metric = read_metric(in);
    variables_ = read_variables(in);
    int num_variables = variables_.size();

    if (std::isdigit(in.peek())) { skip_mutexes(in); }

    initial_state_values_.resize(num_variables);
    check_magic(in, "begin_state");
    for (int i = 0; i < num_variables; ++i) { in >> initial_state_values_[i]; }
    check_magic(in, "end_state");

    for (int i = 0; i < num_variables; ++i) {
        variables_[i].axiom_default_value = initial_state_values_[i];
    }

    goals_ = read_goal(in);
    check_facts(goals_, variables_);
    operators_ = read_actions(in, false, use_metric, variables_);
    axioms_ = read_actions(in, true, use_metric, variables_);
    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    /*
      HACK: We use access g_axiom_evaluators here which assumes
      that this task is completely constructed.
    */
    AxiomEvaluator& axiom_evaluator = g_axiom_evaluators[*this];
    axiom_evaluator.evaluate(initial_state_values_);
}

const ExplicitVariable& RootTask::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables_));
    return variables_[var];
}

const ExplicitEffect&
RootTask::get_effect(int op_id, int effect_id, bool is_axiom) const
{
    const ExplicitOperator& op = get_operator_or_axiom(op_id, is_axiom);
    assert(utils::in_bounds(effect_id, op.effects));
    return op.effects[effect_id];
}

const ExplicitOperator&
RootTask::get_operator_or_axiom(int index, bool is_axiom) const
{
    if (is_axiom) {
        assert(utils::in_bounds(index, axioms_));
        return axioms_[index];
    } else {
        assert(utils::in_bounds(index, operators_));
        return operators_[index];
    }
}

int RootTask::get_num_variables() const
{
    return variables_.size();
}

string RootTask::get_variable_name(int var) const
{
    return get_variable(var).name;
}

int RootTask::get_variable_domain_size(int var) const
{
    return get_variable(var).domain_size;
}

int RootTask::get_variable_axiom_layer(int var) const
{
    return get_variable(var).axiom_layer;
}

int RootTask::get_variable_default_axiom_value(int var) const
{
    return get_variable(var).axiom_default_value;
}

string RootTask::get_fact_name(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
    return get_variable(fact.var).fact_names[fact.value];
}

int RootTask::get_num_axioms() const
{
    return axioms_.size();
}

string RootTask::get_axiom_name(int index) const
{
    return get_operator_or_axiom(index, true).name;
}

int RootTask::get_num_axiom_preconditions(int index) const
{
    return get_operator_or_axiom(index, true).preconditions.size();
}

FactPair RootTask::get_axiom_precondition(int op_index, int fact_index) const
{
    const ExplicitOperator& op = get_operator_or_axiom(op_index, true);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int RootTask::get_num_axiom_effects(int op_index) const
{
    return get_operator_or_axiom(op_index, true).effects.size();
}

int RootTask::get_num_axiom_effect_conditions(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index, true).conditions.size();
}

FactPair RootTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect = get_effect(op_index, eff_index, true);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

FactPair RootTask::get_axiom_effect(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index, true).fact;
}

int RootTask::get_num_operators() const
{
    return operators_.size();
}

int RootTask::get_operator_cost(int index) const
{
    return get_operator_or_axiom(index, false).cost;
}

string RootTask::get_operator_name(int index) const
{
    return get_operator_or_axiom(index, false).name;
}

int RootTask::get_num_operator_preconditions(int index) const
{
    return get_operator_or_axiom(index, false).preconditions.size();
}

FactPair RootTask::get_operator_precondition(int op_index, int fact_index) const
{
    const ExplicitOperator& op = get_operator_or_axiom(op_index, false);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int RootTask::get_num_operator_effects(int op_index) const
{
    return get_operator_or_axiom(op_index, false).effects.size();
}

int RootTask::get_num_operator_effect_conditions(int op_index, int eff_index)
    const
{
    return get_effect(op_index, eff_index, false).conditions.size();
}

FactPair RootTask::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect = get_effect(op_index, eff_index, false);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

FactPair RootTask::get_operator_effect(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index, false).fact;
}

int RootTask::get_num_goals() const
{
    return goals_.size();
}

FactPair RootTask::get_goal_fact(int index) const
{
    assert(utils::in_bounds(index, goals_));
    return goals_[index];
}

vector<int> RootTask::get_initial_state_values() const
{
    return initial_state_values_;
}

std::unique_ptr<AbstractTask> read_root_task(istream& in)
{
    return make_unique<RootTask>(in);
}

} // namespace downward::tasks
