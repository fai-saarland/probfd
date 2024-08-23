#include "probfd/tasks/root_task.h"
#include "probfd/tasks/determinization_task.h"

#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include "downward/tasks/root_task.h"

#include "downward/utils/collections.h"

#include "downward/axioms.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <memory>
#include <set>
#include <vector>

using namespace std;
using utils::ExitCode;

namespace probfd::tasks {

shared_ptr<ProbabilisticTask> g_root_task = nullptr;

namespace {

const auto PRE_FILE_PROB_VERSION = "1";

enum Optimization : unsigned char {
    MINIMIZE_LENGTH = 0,
    MINIMIZE = 1,
    MAXIMIZE = 2
};

struct Metric {
    Optimization optimization;
    bool rewards;
};

struct ExplicitVariable {
    int domain_size;
    string name;
    vector<string> fact_names;
    int axiom_layer;
    int axiom_default_value;

    explicit ExplicitVariable(std::istream& in);
};

struct ConditionalEffect {
    FactPair fact;
    vector<FactPair> conditions;

    ConditionalEffect(int var, int value, vector<FactPair>&& conditions);

    friend bool
    operator<(const ConditionalEffect& left, const ConditionalEffect& right)
    {
        return std::tie(left.fact, left.conditions) <
               std::tie(right.fact, right.conditions);
    }
};

struct ProbabilisticOutcome {
    value_t probability;
    vector<ConditionalEffect> effects;

    ProbabilisticOutcome(std::istream& in);
};

struct ProbabilisticOperator {
    vector<FactPair> preconditions;
    vector<ProbabilisticOutcome> outcomes;
    value_t cost;
    string name;
    int outcomes_start_index;

    ProbabilisticOperator(
        std::istream& in,
        Optimization optimization,
        int& total_num_outcomes);
};

struct ExplicitAxiom {
    vector<FactPair> preconditions;
    vector<ConditionalEffect> effects;
    string name;

    explicit ExplicitAxiom(std::istream& in);

    void read_pre_post(std::istream& in);
};

class RootTask : public ProbabilisticTask {
    vector<ExplicitVariable> variables;
    vector<vector<set<FactPair>>> mutexes;
    vector<ExplicitAxiom> axioms;
    vector<ProbabilisticOperator> operators;
    vector<int> initial_state_values;
    vector<FactPair> goals;

    value_t termination_cost;

    const ExplicitVariable& get_variable(int var) const;
    const ExplicitAxiom& get_axiom(int index) const;
    const ProbabilisticOperator& get_operator(int index) const;
    const ProbabilisticOutcome& get_outcome(int op_id, int outcome_index) const;
    const ConditionalEffect& get_axiom_effect_(int op_id, int effect_id) const;
    const ConditionalEffect&
    get_operator_effect(int op_id, int outcome_index, int effect_id) const;

public:
    explicit RootTask(std::istream& in);

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
    value_t get_operator_cost(int index) const override;
    string get_operator_name(int index) const override;

    int get_num_operator_preconditions(int index) const override;
    FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    value_t get_non_goal_termination_cost() const override;

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
        const PlanningTask* ancestor_task) const override;

    int convert_operator_index(int index, const PlanningTask* ancestor_task)
        const override;
};

class RootTaskCostFunction {
    std::vector<value_t> operator_costs;
    value_t termination_cost;

public:
    value_t get_operator_cost(int index) const;
    value_t get_non_goal_termination_cost() const;
};

void check_fact(const FactPair& fact, const vector<ExplicitVariable>& variables)
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

void check_facts(
    const vector<FactPair>& facts,
    const vector<ExplicitVariable>& variables)
{
    for (FactPair fact : facts) {
        check_fact(fact, variables);
    }
}

void check_facts(
    const ExplicitAxiom& axiom,
    const vector<ExplicitVariable>& variables)
{
    check_facts(axiom.preconditions, variables);

    for (const ConditionalEffect& eff : axiom.effects) {
        check_fact(eff.fact, variables);
        check_facts(eff.conditions, variables);
    }
}

void check_facts(
    const ProbabilisticOperator& action,
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

        for (const ConditionalEffect& eff : outcome.effects) {
            check_fact(eff.fact, variables);
            check_facts(eff.conditions, variables);
        }

        total_prob += prob;
    }

    if (!is_approx_equal(total_prob, 1_vt)) {
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
    for (int i = 0; i < domain_size; ++i) getline(in, fact_names[i]);
    check_magic(in, "end_variable");
}

ConditionalEffect::ConditionalEffect(
    int var,
    int value,
    vector<FactPair>&& conditions)
    : fact(var, value)
    , conditions(std::move(conditions))
{
    std::ranges::sort(this->conditions);
}

ProbabilisticOutcome::ProbabilisticOutcome(std::istream& in)
{
    // Read probability
    std::string p;
    in >> p;
    probability = string_to_value(p);

    // Read number of effects
    int count;
    in >> count;
    effects.reserve(count);

    // Read each effect
    for (int i = 0; i < count; ++i) {
        vector<FactPair> conditions = read_facts(in);
        int var, value_post;
        in >> var >> value_post;
        effects.emplace_back(var, value_post, std::move(conditions));
    }

    std::sort(effects.begin(), effects.end());
}

ProbabilisticOperator::ProbabilisticOperator(
    std::istream& in,
    Optimization optimization,
    int& total_num_outcomes)
    : outcomes_start_index(total_num_outcomes)
{
    check_magic(in, "begin_operator");
    in >> ws;
    getline(in, name);

    // Read preconditions
    this->preconditions = read_facts(in);

    // Read number of outcomes
    int num_outcomes;
    in >> num_outcomes;
    this->outcomes.reserve(num_outcomes);

    total_num_outcomes += num_outcomes;

    if (num_outcomes < 1) {
        std::cerr << "Input file specifies " << num_outcomes
                  << " outcomes for operator " << name << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }

    // Read each outcome
    for (int i = 0; i < num_outcomes; ++i) {
        outcomes.emplace_back(in);
    }

    // Read cost
    std::string cost_text;
    in >> cost_text;

    if (optimization == Optimization::MINIMIZE_LENGTH) {
        // Ignore read value, set to 1
        this->cost = 1;
    } else {
        value_t weight = string_to_value(cost_text);

        // cast maximization objective to minimization objective
        if (optimization == Optimization::MAXIMIZE) weight = -weight;

        this->cost = weight;
    }

    check_magic(in, "end_operator");
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
    int var, pre, value_post;
    in >> var >> pre >> value_post;
    if (pre != -1) {
        preconditions.emplace_back(var, pre);
    }
    effects.emplace_back(var, value_post, std::move(conditions));
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

Metric read_metric(std::istream& in)
{
    int optimization;
    bool rewards;
    check_magic(in, "begin_metric");
    in >> optimization;
    in >> rewards;
    check_magic(in, "end_metric");
    return Metric{static_cast<Optimization>(optimization), rewards};
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

vector<ProbabilisticOperator> read_probabilistic_operators(
    std::istream& in,
    Optimization optimization,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;
    vector<ProbabilisticOperator> actions;
    actions.reserve(count);
    int total_num_outcomes = 0;
    for (int i = 0; i < count; ++i) {
        auto& new_op =
            actions.emplace_back(in, optimization, total_num_outcomes);
        check_facts(new_op, variables);
    }
    return actions;
}

RootTask::RootTask(std::istream& in)
{
    read_and_verify_version(in);
    Metric metric = read_metric(in);
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
    operators =
        read_probabilistic_operators(in, metric.optimization, variables);
    axioms = read_axioms(in, variables);
    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    termination_cost = metric.rewards ? 0_vt : INFINITE_VALUE;

    /*
      HACK: We use a TaskProxy to access g_axiom_evaluators here which
      assumes that this task is completely constructed.
    */
    AxiomEvaluator& axiom_evaluator =
        g_axiom_evaluators[PlanningTaskProxy(*this)];
    axiom_evaluator.evaluate(initial_state_values);
}

const ExplicitVariable& RootTask::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables));
    return variables[var];
}

const ProbabilisticOutcome&
RootTask::get_outcome(int op_id, int outcome_index) const
{
    const ProbabilisticOperator& op = get_operator(op_id);
    assert(utils::in_bounds(outcome_index, op.outcomes));
    return op.outcomes[outcome_index];
}

const ConditionalEffect&
RootTask::get_axiom_effect_(int op_id, int effect_id) const
{
    const ExplicitAxiom& axiom = get_axiom(op_id);
    assert(utils::in_bounds(effect_id, axiom.effects));
    return axiom.effects[effect_id];
}

const ConditionalEffect&
RootTask::get_operator_effect(int op_id, int outcome_index, int effect_id) const
{
    const ProbabilisticOutcome& outcome = get_outcome(op_id, outcome_index);
    assert(utils::in_bounds(effect_id, outcome.effects));
    return outcome.effects[effect_id];
}

const ExplicitAxiom& RootTask::get_axiom(int index) const
{
    assert(utils::in_bounds(index, axioms));
    return axioms[index];
}

const ProbabilisticOperator& RootTask::get_operator(int index) const
{
    assert(utils::in_bounds(index, operators));
    return operators[index];
}

int RootTask::get_num_variables() const
{
    return variables.size();
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

bool RootTask::are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
    const
{
    if (fact1.var == fact2.var) {
        // Same variable: mutex iff different value.
        return fact1.value != fact2.value;
    }
    assert(utils::in_bounds(fact1.var, mutexes));
    assert(utils::in_bounds(fact1.value, mutexes[fact1.var]));
    return bool(mutexes[fact1.var][fact1.value].count(fact2));
}

int RootTask::get_num_axioms() const
{
    return axioms.size();
}

string RootTask::get_axiom_name(int index) const
{
    return get_axiom(index).name;
}

int RootTask::get_num_axiom_preconditions(int index) const
{
    return get_axiom(index).preconditions.size();
}

FactPair RootTask::get_axiom_precondition(int op_index, int fact_index) const
{
    const ExplicitAxiom& axiom = get_axiom(op_index);
    assert(utils::in_bounds(fact_index, axiom.preconditions));
    return axiom.preconditions[fact_index];
}

int RootTask::get_num_axiom_effects(int op_index) const
{
    return get_axiom(op_index).effects.size();
}

FactPair RootTask::get_axiom_effect(int op_index, int eff_index) const
{
    return get_axiom_effect_(op_index, eff_index).fact;
}

int RootTask::get_num_axiom_effect_conditions(int op_index, int eff_index) const
{
    return get_axiom_effect_(op_index, eff_index).conditions.size();
}

FactPair RootTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ConditionalEffect& effect = get_axiom_effect_(op_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

int RootTask::get_num_operators() const
{
    return operators.size();
}

value_t RootTask::get_operator_cost(int index) const
{
    return static_cast<value_t>(get_operator(index).cost);
}

string RootTask::get_operator_name(int index) const
{
    return get_operator(index).name;
}

int RootTask::get_num_operator_preconditions(int index) const
{
    return get_operator(index).preconditions.size();
}

FactPair RootTask::get_operator_precondition(int op_index, int fact_index) const
{
    const ProbabilisticOperator& op = get_operator(op_index);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

value_t RootTask::get_non_goal_termination_cost() const
{
    return termination_cost;
}

int RootTask::get_num_operator_outcomes(int index) const
{
    return get_operator(index).outcomes.size();
}

value_t
RootTask::get_operator_outcome_probability(int index, int outcome_index) const
{
    return get_outcome(index, outcome_index).probability;
}

int RootTask::get_operator_outcome_id(int index, int outcome_index) const
{
    return get_operator(index).outcomes_start_index + outcome_index;
}

int RootTask::get_num_operator_outcome_effects(int op_index, int outcome_index)
    const
{
    return get_outcome(op_index, outcome_index).effects.size();
}

FactPair RootTask::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index).fact;
}

int RootTask::get_num_operator_outcome_effect_conditions(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index)
        .conditions.size();
}

FactPair RootTask::get_operator_outcome_effect_condition(
    int op_index,
    int outcome_index,
    int eff_index,
    int cond_index) const
{
    const ConditionalEffect& effect =
        get_operator_effect(op_index, outcome_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

int RootTask::get_num_goals() const
{
    return goals.size();
}

FactPair RootTask::get_goal_fact(int index) const
{
    assert(utils::in_bounds(index, goals));
    return goals[index];
}

vector<int> RootTask::get_initial_state_values() const
{
    return initial_state_values;
}

void RootTask::convert_ancestor_state_values(
    vector<int>&,
    const PlanningTask* ancestor_task) const
{
    if (this != ancestor_task) {
        ABORT("Invalid state conversion");
    }
}

int RootTask::convert_operator_index(
    int index,
    const PlanningTask* ancestor_task) const
{
    if (this != ancestor_task) {
        ABORT("Invalid operator ID conversion");
    }
    return index;
}

} // namespace

std::unique_ptr<ProbabilisticTask> read_sas_task(std::istream& in)
{
    return std::make_unique<RootTask>(in);
}

std::shared_ptr<ProbabilisticTask> read_root_tasks(std::istream& in)
{
    std::shared_ptr<ProbabilisticTask> input_task = read_sas_task(in);
    set_root_task(input_task);
    return input_task;
}

void set_root_task(std::shared_ptr<ProbabilisticTask> task)
{
    // FIXME crashes in tests since it persists in between tests.
    // assert(!g_root_task);
    ::tasks::g_root_task = std::make_shared<DeterminizationTask>(task);
    g_root_task = std::move(task);
}

} // namespace probfd::tasks
