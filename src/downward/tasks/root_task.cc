#include "downward/tasks/root_task.h"

#include "downward/abstract_task.h"
#include "downward/initial_state_values.h"
#include "downward/state_registry.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include "downward/operator_cost_function.h"

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

    explicit ExplicitVariable(istream& in, int& axiom_layer);
};

struct ExplicitVariableAxiomInfo {
    int axiom_layer;
    int axiom_default_value;
};

struct ExplicitEffect {
    FactPair fact;
    vector<FactPair> conditions;

    ExplicitEffect(int var, int value, vector<FactPair>&& conditions);
};

template <bool b>
struct ExplicitOperatorOrAxiom {
    vector<FactPair> preconditions;
    vector<ExplicitEffect> effects;
    string name;

    void read_pre_post(istream& in);

    ExplicitOperatorOrAxiom(istream& in)
        requires(b);

    ExplicitOperatorOrAxiom(istream& in, bool use_metric, int& cost)
        requires(!b);
};

using ExplicitAxiom = ExplicitOperatorOrAxiom<true>;
using ExplicitOperator = ExplicitOperatorOrAxiom<false>;

class RootVariableSpace : public VariableSpace {
    vector<ExplicitVariable> variables_;

    const ExplicitVariable& get_variable(int var) const;

public:
    explicit RootVariableSpace(vector<ExplicitVariable> variables);

    int get_num_variables() const override;
    string get_variable_name(int var) const override;
    int get_variable_domain_size(int var) const override;
    string get_fact_name(const FactPair& fact) const override;
};

class RootInitialStateValues : public InitialStateValues {
    vector<int> initial_state_values_;

public:
    explicit RootInitialStateValues(vector<int> initial_state_values);

    vector<int> get_initial_state_values() const override;
};

class RootGoal : public GoalFactList {
    vector<FactPair> goals_;

public:
    explicit RootGoal(vector<FactPair> goals);

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;
};

class RootAxiomSpace : public AxiomSpace {
    vector<ExplicitVariableAxiomInfo> variables_infos_;
    vector<ExplicitAxiom> axioms_;

    const ExplicitEffect& get_effect(int ax_id, int effect_id) const;
    const ExplicitAxiom& get_axiom(int index) const;

public:
    explicit RootAxiomSpace(
        vector<ExplicitVariableAxiomInfo> variables_infos,
        vector<ExplicitAxiom> axioms);

    int get_variable_axiom_layer(int var) const override;
    int get_variable_default_axiom_value(int var) const override;

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
};

class RootOperatorSpace : public ClassicalOperatorSpace {
    vector<ExplicitOperator> operators_;

    const ExplicitEffect& get_effect(int op_id, int effect_id) const;
    const ExplicitOperator& get_operator(int index) const;

public:
    explicit RootOperatorSpace(vector<ExplicitOperator> operators);

    int get_num_operators() const override;

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
};

class RootOperatorCostFunction : public OperatorIntCostFunction {
    vector<int> costs_;

public:
    explicit RootOperatorCostFunction(vector<int> costs)
        : costs_(std::move(costs))
    {
    }

    int get_operator_cost(int index) const override;
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

static void
check_facts(const auto& action, const vector<ExplicitVariable>& variables)
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

ExplicitVariable::ExplicitVariable(istream& in, int& axiom_layer)
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

template <bool b>
void ExplicitOperatorOrAxiom<b>::read_pre_post(istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, value_pre, value_post;
    in >> var >> value_pre >> value_post;
    if (value_pre != -1) { preconditions.emplace_back(var, value_pre); }
    effects.emplace_back(var, value_post, std::move(conditions));
}

template <bool b>
ExplicitOperatorOrAxiom<b>::ExplicitOperatorOrAxiom(istream& in)
    requires(b)
{
    name = "<axiom>";
    check_magic(in, "begin_rule");
    read_pre_post(in);
    check_magic(in, "end_rule");
}

template <bool b>
ExplicitOperatorOrAxiom<b>::ExplicitOperatorOrAxiom(
    istream& in,
    bool use_metric,
    int& cost)
    requires(!b)
{

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

struct VariableInfo {
    vector<ExplicitVariable> domains;
    vector<ExplicitVariableAxiomInfo> axiom_infos;
};

static VariableInfo read_variables(istream& in)
{
    int count;
    in >> count;
    VariableInfo variables;
    variables.domains.reserve(count);
    variables.axiom_infos.reserve(count);

    for (int i = 0; i < count; ++i) {
        auto& axiom_info = variables.axiom_infos.emplace_back();
        variables.domains.emplace_back(in, axiom_info.axiom_layer);
    }

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

struct OperatorInfo {
    std::vector<ExplicitOperator> operators;
    std::vector<int> costs;
};

static std::vector<ExplicitAxiom>
read_axioms(istream& in, const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitAxiom> axioms;
    axioms.reserve(count);
    for (int i = 0; i < count; ++i) {
        axioms.emplace_back(in);
        check_facts(axioms.back(), variables);
    }
    return std::move(axioms);
}

static OperatorInfo read_actions(
    istream& in,
    bool use_metric,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ExplicitOperator> actions;
    vector<int> costs;
    actions.reserve(count);
    costs.reserve(count);
    for (int i = 0; i < count; ++i) {
        actions.emplace_back(in, use_metric, costs.emplace_back());
        check_facts(actions.back(), variables);
    }
    return {std::move(actions), std::move(costs)};
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

RootVariableSpace::RootVariableSpace(vector<ExplicitVariable> variables)
    : variables_(std::move(variables))
{
}

RootInitialStateValues::RootInitialStateValues(vector<int> initial_state_values)
    : initial_state_values_(std::move(initial_state_values))
{
}

RootAxiomSpace::RootAxiomSpace(
    vector<ExplicitVariableAxiomInfo> variables_infos,
    vector<ExplicitAxiom> axioms)
    : variables_infos_(std::move(variables_infos))
    , axioms_(std::move(axioms))
{
}

RootOperatorSpace::RootOperatorSpace(vector<ExplicitOperator> operators)
    : operators_(std::move(operators))
{
}

RootGoal::RootGoal(vector<FactPair> goals)
    : goals_(std::move(goals))
{
}

const ExplicitVariable& RootVariableSpace::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables_));
    return variables_[var];
}

const ExplicitEffect& RootAxiomSpace::get_effect(int op_id, int effect_id) const
{
    const ExplicitAxiom& ax = get_axiom(op_id);
    assert(utils::in_bounds(effect_id, ax.effects));
    return ax.effects[effect_id];
}

const ExplicitAxiom& RootAxiomSpace::get_axiom(int index) const
{
    assert(utils::in_bounds(index, axioms_));
    return axioms_[index];
}

const ExplicitEffect&
RootOperatorSpace::get_effect(int op_id, int effect_id) const
{
    const ExplicitOperator& op = get_operator(op_id);
    assert(utils::in_bounds(effect_id, op.effects));
    return op.effects[effect_id];
}

const ExplicitOperator& RootOperatorSpace::get_operator(int index) const
{
    assert(utils::in_bounds(index, operators_));
    return operators_[index];
}

int RootVariableSpace::get_num_variables() const
{
    return variables_.size();
}

string RootVariableSpace::get_variable_name(int var) const
{
    return get_variable(var).name;
}

int RootVariableSpace::get_variable_domain_size(int var) const
{
    return get_variable(var).domain_size;
}

string RootVariableSpace::get_fact_name(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
    return get_variable(fact.var).fact_names[fact.value];
}

int RootAxiomSpace::get_variable_axiom_layer(int var) const
{
    return variables_infos_[var].axiom_layer;
}

int RootAxiomSpace::get_variable_default_axiom_value(int var) const
{
    return variables_infos_[var].axiom_default_value;
}

int RootAxiomSpace::get_num_axioms() const
{
    return axioms_.size();
}

string RootAxiomSpace::get_axiom_name(int index) const
{
    return get_axiom(index).name;
}

int RootAxiomSpace::get_num_axiom_preconditions(int index) const
{
    return get_axiom(index).preconditions.size();
}

FactPair
RootAxiomSpace::get_axiom_precondition(int op_index, int fact_index) const
{
    const ExplicitAxiom& op = get_axiom(op_index);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int RootAxiomSpace::get_num_axiom_effects(int op_index) const
{
    return get_axiom(op_index).effects.size();
}

int RootAxiomSpace::get_num_axiom_effect_conditions(int op_index, int eff_index)
    const
{
    return get_effect(op_index, eff_index).conditions.size();
}

FactPair RootAxiomSpace::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect = get_effect(op_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

FactPair RootAxiomSpace::get_axiom_effect(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index).fact;
}

int RootOperatorSpace::get_num_operators() const
{
    return operators_.size();
}

string RootOperatorSpace::get_operator_name(int index) const
{
    return get_operator(index).name;
}

int RootOperatorSpace::get_num_operator_preconditions(int index) const
{
    return get_operator(index).preconditions.size();
}

FactPair
RootOperatorSpace::get_operator_precondition(int op_index, int fact_index) const
{
    const ExplicitOperator& op = get_operator(op_index);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

int RootOperatorSpace::get_num_operator_effects(int op_index) const
{
    return get_operator(op_index).effects.size();
}

int RootOperatorSpace::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    return get_effect(op_index, eff_index).conditions.size();
}

FactPair RootOperatorSpace::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ExplicitEffect& effect = get_effect(op_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

FactPair
RootOperatorSpace::get_operator_effect(int op_index, int eff_index) const
{
    return get_effect(op_index, eff_index).fact;
}

int RootGoal::get_num_goals() const
{
    return goals_.size();
}

FactPair RootGoal::get_goal_fact(int index) const
{
    assert(utils::in_bounds(index, goals_));
    return goals_[index];
}

vector<int> RootInitialStateValues::get_initial_state_values() const
{
    return initial_state_values_;
}

int RootOperatorCostFunction::get_operator_cost(int index) const
{
    return costs_[index];
}

UniqueAbstractTask read_root_task(istream& in)
{
    read_and_verify_version(in);
    bool use_metric = read_metric(in);
    auto variable_info = read_variables(in);

    int num_variables = variable_info.domains.size();

    if (std::isdigit((in >> std::ws).peek())) { skip_mutexes(in); }

    std::vector<int> initial_state;
    initial_state.reserve(num_variables);

    check_magic(in, "begin_state");
    for (int i = 0; i < num_variables; ++i) {
        in >> initial_state.emplace_back();
    }
    check_magic(in, "end_state");

    for (int i = 0; i < num_variables; ++i) {
        variable_info.axiom_infos[i].axiom_default_value = initial_state[i];
    }

    auto goal_facts = read_goal(in);

    check_facts(goal_facts, variable_info.domains);

    auto action_info = read_actions(in, use_metric, variable_info.domains);
    auto axiom_info = read_axioms(in, variable_info.domains);

    // Construct interfaces
    auto variables =
        std::make_unique<RootVariableSpace>(std::move(variable_info.domains));

    auto axioms = std::make_unique<RootAxiomSpace>(
        std::move(variable_info.axiom_infos),
        std::move(axiom_info));

    auto operators =
        std::make_unique<RootOperatorSpace>(std::move(action_info.operators));

    auto goals = std::make_unique<RootGoal>(std::move(goal_facts));
    auto cost_function = std::make_unique<RootOperatorCostFunction>(
        std::move(action_info.costs));

    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    AxiomEvaluator& axiom_evaluator = g_axiom_evaluators[*variables, *axioms];
    axiom_evaluator.evaluate(initial_state);

    auto initial_state_values =
        std::make_unique<RootInitialStateValues>(std::move(initial_state));

    return std::forward_as_tuple(
        std::move(variables),
        std::move(axioms),
        std::move(operators),
        std::move(goals),
        std::move(initial_state_values),
        std::move(cost_function));
}

} // namespace downward::tasks
