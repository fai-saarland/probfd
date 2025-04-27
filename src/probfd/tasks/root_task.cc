#include "probfd/tasks/root_task.h"
#include "probfd/tasks/determinization_task.h"

#include "probfd/probabilistic_task.h"
#include "probfd/termination_costs.h"
#include "probfd/value_type.h"

#include "downward/tasks/root_task.h"

#include "downward/axiom_evaluator.h"
#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#include "downward/axiom_space.h"
#include "downward/axioms.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/per_task_information.h"
#include "downward/variable_space.h"
#include "probfd/probabilistic_operator_space.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

using namespace std;

using namespace downward;

using utils::ExitCode;

namespace probfd::tasks {

namespace {

const auto PRE_FILE_PROB_VERSION = "1";

enum Optimization : unsigned char {
    MINIMIZE_LENGTH = 0,
    MINIMIZE = 1,
    MAXIMIZE = 2
};

struct Fraction {
    int numerator;
    int denominator;

    friend Fraction operator+(const Fraction& left, const Fraction& right);

    Fraction& operator+=(const Fraction& other)
    {
        return *this = (*this + other);
    }
};

Fraction operator+(const Fraction& left, const Fraction& right)
{
    const int lcm = std::lcm(left.denominator, right.denominator);
    const int numerator = (left.numerator * (lcm / left.denominator)) +
                          (right.numerator * (lcm / right.denominator));
    return {numerator, lcm};
}

struct Metric {
    Optimization optimization;
    bool rewards;
    std::optional<value_t> goal_reward;
};

struct ExplicitVariable {
    int domain_size;
    string name;
    vector<string> fact_names;

    explicit ExplicitVariable(std::istream& in, int& axiom_layer);
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
    Fraction fractional_probability;
    value_t probability;
    vector<ConditionalEffect> effects;

    explicit ProbabilisticOutcome(std::istream& in);
};

struct ProbabilisticOperator {
    vector<FactPair> preconditions;
    vector<ProbabilisticOutcome> outcomes;
    string name;
    int outcomes_start_index;

    ProbabilisticOperator(
        std::istream& in,
        Optimization optimization,
        int& total_num_outcomes,
        value_t& cost);
};

struct ExplicitAxiom {
    vector<FactPair> preconditions;
    vector<ConditionalEffect> effects;
    string name;

    explicit ExplicitAxiom(std::istream& in);

    void read_pre_post(std::istream& in);
};

class RootVariableSpace : public VariableSpace {
    vector<ExplicitVariable> variables_;

    const ExplicitVariable& get_variable(int var) const;

public:
    explicit RootVariableSpace(vector<ExplicitVariable> variables)
        : variables_(std::move(variables))
    {
    }

    int get_num_variables() const override;
    string get_variable_name(int var) const override;
    int get_variable_domain_size(int var) const override;
    string get_fact_name(const FactPair& fact) const override;
};

class RootInitialStateValues : public InitialStateValues {
    vector<int> initial_state_values_;

public:
    explicit RootInitialStateValues(vector<int> initial_state_values)
        : initial_state_values_(std::move(initial_state_values))
    {
    }

    vector<int> get_initial_state_values() const override;
};

class RootGoal : public GoalFactList {
    vector<FactPair> goals_;

public:
    explicit RootGoal(vector<FactPair> goals)
        : goals_(std::move(goals))
    {
    }

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;
};

class RootAxiomSpace : public AxiomSpace {
    vector<ExplicitVariableAxiomInfo> variables_infos_;
    vector<ExplicitAxiom> axioms_;

    const ConditionalEffect& get_axiom_effect_(int op_id, int effect_id) const;
    const ExplicitAxiom& get_axiom(int index) const;

public:
    explicit RootAxiomSpace(
        vector<ExplicitVariableAxiomInfo> variables_infos,
        vector<ExplicitAxiom> axioms)
        : variables_infos_(std::move(variables_infos))
        , axioms_(std::move(axioms))
    {
    }

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

class RootOperatorSpace : public ProbabilisticOperatorSpace {
    vector<ProbabilisticOperator> operators_;

    const ExplicitEffect& get_effect(int op_id, int effect_id) const;
    const ProbabilisticOperator& get_operator(int index) const;
    const ProbabilisticOutcome& get_outcome(int op_id, int outcome_index) const;
    const ConditionalEffect&
    get_operator_effect(int op_id, int outcome_index, int effect_id) const;

public:
    explicit RootOperatorSpace(vector<ProbabilisticOperator> operators)
        : operators_(std::move(operators))
    {
    }

    int get_num_operators() const override;
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
};

class RootOperatorCostFunction : public OperatorCostFunction<value_t> {
    vector<value_t> costs_;

public:
    explicit RootOperatorCostFunction(vector<value_t> costs)
        : costs_(costs)
    {
    }

    value_t get_operator_cost(int index) const override;
};

class RootTerminationCostFunction : public TerminationCosts {
    value_t goal_termination_cost;
    value_t non_goal_termination_cost;

public:
    explicit RootTerminationCostFunction(
        value_t goal_termination_cost,
        value_t non_goal_termination_cost)
        : goal_termination_cost(goal_termination_cost)
        , non_goal_termination_cost(non_goal_termination_cost)
    {
    }

    value_t get_goal_termination_cost() const override;
    value_t get_non_goal_termination_cost() const override;
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
    for (FactPair fact : facts) { check_fact(fact, variables); }
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

    Fraction total_prob = {0, 1};

    for (const auto& outcome : action.outcomes) {
        const auto prob = outcome.fractional_probability;

        for (const ConditionalEffect& eff : outcome.effects) {
            check_fact(eff.fact, variables);
            check_facts(eff.conditions, variables);
        }

        total_prob += prob;
    }

    if (total_prob.numerator != total_prob.denominator) {
        cerr << "Total outcome probabilities must sum up to one. Sum was: "
             << total_prob.numerator << "/" << total_prob.denominator << endl;
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

ExplicitVariable::ExplicitVariable(std::istream& in, int& axiom_layer)
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

    auto it = p.find('/');

    if (it == std::string::npos) {
        fractional_probability.numerator = std::stoi(p);
        fractional_probability.denominator = 1;
    } else {
        std::string numerator = p.substr(0, it);
        std::string denominator = p.substr(it + 1);

        fractional_probability.numerator = std::stoi(numerator);
        fractional_probability.denominator = std::stoi(denominator);

        if (fractional_probability.denominator < 0) {
            cerr << "Read probability with denominator of zero: " << p << endl;
            utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
        }
    }

    if (fractional_probability.numerator < 0) {
        fractional_probability.numerator = -fractional_probability.numerator;
        fractional_probability.denominator =
            -fractional_probability.denominator;
    }

    if (fractional_probability.numerator < 0) {
        cerr << "Probability must be grater than zero: " << p << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    if (fractional_probability.numerator > fractional_probability.denominator) {
        cerr << "Probability must be less or equal to one: " << p << endl;
        utils::exit_with(ExitCode::SEARCH_INPUT_ERROR);
    }

    const int gcd = std::gcd(
        fractional_probability.numerator,
        fractional_probability.denominator);

    fractional_probability.numerator /= gcd;
    fractional_probability.denominator /= gcd;

    probability = fraction_to_value(
        fractional_probability.numerator,
        fractional_probability.denominator);

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
    int& total_num_outcomes,
    value_t& cost)
    : outcomes_start_index(total_num_outcomes)
{
    check_magic(in, "begin_operator");
    in >> ws;
    getline(in, name);

    // Read preconditions
    this->preconditions = read_facts(in);
    std::ranges::sort(this->preconditions);

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
    for (int i = 0; i < num_outcomes; ++i) { outcomes.emplace_back(in); }

    // Read cost
    std::string cost_text;
    in >> cost_text;

    if (optimization == Optimization::MINIMIZE_LENGTH) {
        // Ignore read value, set to 1
        cost = 1;
    } else {
        value_t weight = string_to_value(cost_text);

        // cast maximization objective to minimization objective
        if (optimization == Optimization::MAXIMIZE) weight = -weight;

        cost = weight;
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
    for (int i = 0; i < count; ++i) { read_pre_post(in); }
    check_magic(in, "end_rule");
}

void ExplicitAxiom::read_pre_post(std::istream& in)
{
    vector<FactPair> conditions = read_facts(in);
    int var, pre, value_post;
    in >> var >> pre >> value_post;
    if (pre != -1) { preconditions.emplace_back(var, pre); }
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
    std::optional<value_t> goal_reward;
    check_magic(in, "begin_metric");
    in >> optimization;
    in >> rewards;
    if (rewards) {
        bool g;
        in >> g;
        goal_reward = g;
    }
    check_magic(in, "end_metric");
    return Metric{
        static_cast<Optimization>(optimization),
        rewards,
        goal_reward};
}

struct VariableInfo {
    vector<ExplicitVariable> domains;
    vector<ExplicitVariableAxiomInfo> axiom_infos;
};

VariableInfo read_variables(std::istream& in)
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

struct OperatorInfo {
    std::vector<ProbabilisticOperator> operators;
    std::vector<value_t> costs;
};

OperatorInfo read_probabilistic_operators(
    std::istream& in,
    Optimization optimization,
    const vector<ExplicitVariable>& variables)
{
    int count;
    in >> count;

    vector<ProbabilisticOperator> actions;
    vector<value_t> costs;
    actions.reserve(count);
    costs.reserve(count);

    int total_num_outcomes = 0;
    for (int i = 0; i < count; ++i) {
        auto& new_op = actions.emplace_back(
            in,
            optimization,
            total_num_outcomes,
            costs.emplace_back());
        check_facts(new_op, variables);
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

const ExplicitVariable& RootVariableSpace::get_variable(int var) const
{
    assert(utils::in_bounds(var, variables_));
    return variables_[var];
}

const ProbabilisticOutcome&
RootOperatorSpace::get_outcome(int op_id, int outcome_index) const
{
    const ProbabilisticOperator& op = get_operator(op_id);
    assert(utils::in_bounds(outcome_index, op.outcomes));
    return op.outcomes[outcome_index];
}

const ConditionalEffect&
RootAxiomSpace::get_axiom_effect_(int op_id, int effect_id) const
{
    const ExplicitAxiom& axiom = get_axiom(op_id);
    assert(utils::in_bounds(effect_id, axiom.effects));
    return axiom.effects[effect_id];
}

const ConditionalEffect& RootOperatorSpace::get_operator_effect(
    int op_id,
    int outcome_index,
    int effect_id) const
{
    const ProbabilisticOutcome& outcome = get_outcome(op_id, outcome_index);
    assert(utils::in_bounds(effect_id, outcome.effects));
    return outcome.effects[effect_id];
}

const ExplicitAxiom& RootAxiomSpace::get_axiom(int index) const
{
    assert(utils::in_bounds(index, axioms_));
    return axioms_[index];
}

const ProbabilisticOperator& RootOperatorSpace::get_operator(int index) const
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

int RootAxiomSpace::get_variable_axiom_layer(int var) const
{
    return variables_infos_[var].axiom_layer;
}

int RootAxiomSpace::get_variable_default_axiom_value(int var) const
{
    return variables_infos_[var].axiom_default_value;
}

string RootVariableSpace::get_fact_name(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.value, get_variable(fact.var).fact_names));
    return get_variable(fact.var).fact_names[fact.value];
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
    const ExplicitAxiom& axiom = get_axiom(op_index);
    assert(utils::in_bounds(fact_index, axiom.preconditions));
    return axiom.preconditions[fact_index];
}

int RootAxiomSpace::get_num_axiom_effects(int op_index) const
{
    return get_axiom(op_index).effects.size();
}

FactPair RootAxiomSpace::get_axiom_effect(int op_index, int eff_index) const
{
    return get_axiom_effect_(op_index, eff_index).fact;
}

int RootAxiomSpace::get_num_axiom_effect_conditions(int op_index, int eff_index)
    const
{
    return get_axiom_effect_(op_index, eff_index).conditions.size();
}

FactPair RootAxiomSpace::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const ConditionalEffect& effect = get_axiom_effect_(op_index, eff_index);
    assert(utils::in_bounds(cond_index, effect.conditions));
    return effect.conditions[cond_index];
}

value_t RootOperatorCostFunction::get_operator_cost(int index) const
{
    return costs_[index];
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
    const ProbabilisticOperator& op = get_operator(op_index);
    assert(utils::in_bounds(fact_index, op.preconditions));
    return op.preconditions[fact_index];
}

value_t RootTerminationCostFunction::get_goal_termination_cost() const
{
    return goal_termination_cost;
}

value_t RootTerminationCostFunction::get_non_goal_termination_cost() const
{
    return non_goal_termination_cost;
}

int RootOperatorSpace::get_num_operator_outcomes(int index) const
{
    return get_operator(index).outcomes.size();
}

value_t RootOperatorSpace::get_operator_outcome_probability(
    int index,
    int outcome_index) const
{
    return get_outcome(index, outcome_index).probability;
}

int RootOperatorSpace::get_operator_outcome_id(int index, int outcome_index)
    const
{
    return get_operator(index).outcomes_start_index + outcome_index;
}

int RootOperatorSpace::get_num_operator_outcome_effects(
    int op_index,
    int outcome_index) const
{
    return get_outcome(op_index, outcome_index).effects.size();
}

FactPair RootOperatorSpace::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index).fact;
}

int RootOperatorSpace::get_num_operator_outcome_effect_conditions(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_operator_effect(op_index, outcome_index, eff_index)
        .conditions.size();
}

FactPair RootOperatorSpace::get_operator_outcome_effect_condition(
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

} // namespace

UniqueProbabilisticTask read_sas_task(std::istream& in)
{
    read_and_verify_version(in);
    Metric metric = read_metric(in);
    auto variable_info = read_variables(in);
    int num_variables = variable_info.domains.size();

    if (const int c = (in >> std::ws).peek(); std::isdigit(c)) {
        skip_mutexes(in);
    }

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

    auto action_info = read_probabilistic_operators(
        in,
        metric.optimization,
        variable_info.domains);
    auto axiom_info = read_axioms(in, variable_info.domains);

    /* TODO: We should be stricter here and verify that we
       have reached the end of "in". */

    value_t goal_termination_cost;
    value_t non_goal_termination_cost;

    if (metric.rewards) {
        assert(metric.goal_reward.has_value());
        goal_termination_cost = *metric.goal_reward;
        non_goal_termination_cost = 0_vt;
    } else {
        goal_termination_cost = 0_vt;
        non_goal_termination_cost = INFINITE_VALUE;
    }

    if (metric.optimization == Optimization::MAXIMIZE) {
        goal_termination_cost = -goal_termination_cost;
        non_goal_termination_cost = -non_goal_termination_cost;
    }

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

    auto term_costs = std::make_unique<RootTerminationCostFunction>(
        goal_termination_cost,
        non_goal_termination_cost);

    /*
      HACK: We use access g_axiom_evaluators here which
      assumes that this task is completely constructed.
    */
    AxiomEvaluator& axiom_evaluator =
        downward::g_axiom_evaluators[*variables, *axioms];
    axiom_evaluator.evaluate(initial_state);

    auto initial_state_values =
        std::make_unique<RootInitialStateValues>(std::move(initial_state));

    return std::forward_as_tuple(
        std::move(variables),
        std::move(axioms),
        std::move(operators),
        std::move(goals),
        std::move(initial_state_values),
        std::move(cost_function),
        std::move(term_costs));
}

UniqueProbabilisticTask read_sas_task(const std::filesystem::path& filepath)
{
    std::fstream input_file(filepath);
    return read_sas_task(input_file);
}

UniqueProbabilisticTask
read_sas_task_from_file(const std::filesystem::path& filepath)
{
    return read_sas_task(filepath);
}

} // namespace probfd::tasks
