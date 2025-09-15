#include "downward/merge_and_shrink/fts_factory.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/labels.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"
#include "downward/merge_and_shrink/types.h"

#include "downward/abstract_task.h"
#include "downward/classical_operator_space.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/operator_cost_function.h"
#include "downward/operator_cost_function_fwd.h"
#include "downward/state.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

using namespace std;

namespace downward::merge_and_shrink {
class FTSFactory {
    AbstractTaskTuple task;

    struct TransitionSystemData {
        // The following two attributes are only used for statistics
        int num_variables;
        vector<int> incorporated_variables;

        vector<int> label_to_local_label;
        vector<LocalLabelInfo> local_label_infos;
        vector<bool> relevant_labels;
        int num_states;
        vector<bool> goal_states;
        int init_state;

        TransitionSystemData(TransitionSystemData&& other) noexcept
            : num_variables(other.num_variables)
            , incorporated_variables(std::move(other.incorporated_variables))
            , label_to_local_label(std::move(other.label_to_local_label))
            , local_label_infos(std::move(other.local_label_infos))
            , relevant_labels(std::move(other.relevant_labels))
            , num_states(other.num_states)
            , goal_states(std::move(other.goal_states))
            , init_state(other.init_state)
        {
        }

        TransitionSystemData() = default;
        TransitionSystemData(TransitionSystemData& other) = delete;
        TransitionSystemData& operator=(TransitionSystemData& other) = delete;
    };

    vector<TransitionSystemData> transition_system_data_by_var;
    // see TODO in build_transitions()
    int task_has_conditional_effects;

    unique_ptr<Labels> create_labels();
    void build_state_data(VariableProxy var);
    void initialize_transition_system_data(const Labels& labels);
    bool is_relevant(int var_id, int label) const;
    void mark_as_relevant(int var_id, int label);
    unordered_map<int, int> compute_preconditions(OperatorProxy op);
    void handle_operator_effect(
        const VariableSpace& variables,
        OperatorProxy op,
        OperatorEffectProxy effect,
        const unordered_map<int, int>& pre_val,
        vector<bool>& has_effect_on_var,
        vector<vector<Transition>>& transitions_by_var);
    void handle_operator_precondition(
        OperatorProxy op,
        FactPair precondition,
        const vector<bool>& has_effect_on_var,
        vector<vector<Transition>>& transitions_by_var);
    void build_transitions_for_operator(
        const VariableSpace& variables,
        OperatorProxy op);
    void build_transitions_for_irrelevant_ops(
        const VariableProxy& variable,
        const Labels& labels);
    void build_transitions(const Labels& labels);
    vector<unique_ptr<TransitionSystem>>
    create_transition_systems(const Labels& labels);
    vector<unique_ptr<MergeAndShrinkRepresentation>>
    create_mas_representations() const;
    vector<unique_ptr<Distances>> create_distances(
        const vector<unique_ptr<TransitionSystem>>& transition_systems) const;

public:
    explicit FTSFactory(const AbstractTaskTuple& task);
    ~FTSFactory();

    /*
      Note: create() may only be called once. We don't worry about
      misuse because the class is only used internally in this file.
    */
    FactoredTransitionSystem create(
        bool compute_init_distances,
        bool compute_goal_distances,
        utils::LogProxy& log);
};

FTSFactory::FTSFactory(const AbstractTaskTuple& task)
    : task(task)
    , task_has_conditional_effects(false)
{
}

FTSFactory::~FTSFactory()
{
}

unique_ptr<Labels> FTSFactory::create_labels()
{
    const auto& [operators, cost_function] =
        slice<ClassicalOperatorSpace&, OperatorIntCostFunction&>(task);

    vector<int> label_costs;
    int num_ops = operators.size();
    int max_num_labels = 0;
    if (num_ops > 0) {
        max_num_labels = 2 * num_ops - 1;
        label_costs.reserve(max_num_labels);
        for (OperatorProxy op : operators) {
            label_costs.push_back(cost_function.get_operator_cost(op.get_id()));
        }
    }
    return std::make_unique<Labels>(std::move(label_costs), max_num_labels);
}

void FTSFactory::build_state_data(VariableProxy var)
{
    const auto& [variables, goals, init_values] =
        slice<VariableSpace&, GoalFactList&, InitialStateValues&>(task);

    int var_id = var.get_id();
    TransitionSystemData& ts_data = transition_system_data_by_var[var_id];
    ts_data.init_state = init_values.get_initial_state()[var_id];

    int range = variables[var_id].get_domain_size();
    ts_data.num_states = range;

    int goal_value = -1;
    for (const auto [var, value] : goals) {
        if (var == var_id) {
            assert(goal_value == -1);
            goal_value = value;
            break;
        }
    }

    ts_data.goal_states.resize(range, false);
    for (int value = 0; value < range; ++value) {
        if (value == goal_value || goal_value == -1) {
            ts_data.goal_states[value] = true;
        }
    }
}

void FTSFactory::initialize_transition_system_data(const Labels& labels)
{
    const auto& variables = get_variables(task);
    transition_system_data_by_var.resize(variables.size());
    for (VariableProxy var : variables) {
        TransitionSystemData& ts_data =
            transition_system_data_by_var[var.get_id()];
        ts_data.num_variables = variables.size();
        ts_data.incorporated_variables.push_back(var.get_id());
        ts_data.label_to_local_label.resize(labels.get_max_num_labels(), -1);
        ts_data.relevant_labels.resize(labels.get_num_total_labels(), false);
        build_state_data(var);
    }
}

bool FTSFactory::is_relevant(int var_id, int label) const
{
    return transition_system_data_by_var[var_id].relevant_labels[label];
}

void FTSFactory::mark_as_relevant(int var_id, int label)
{
    transition_system_data_by_var[var_id].relevant_labels[label] = true;
}

unordered_map<int, int> FTSFactory::compute_preconditions(OperatorProxy op)
{
    unordered_map<int, int> pre_val;
    for (const auto [var, value] : op.get_preconditions()) pre_val[var] = value;
    return pre_val;
}

void FTSFactory::handle_operator_effect(
    const VariableSpace& variables,
    OperatorProxy op,
    OperatorEffectProxy effect,
    const unordered_map<int, int>& pre_val,
    vector<bool>& has_effect_on_var,
    vector<vector<Transition>>& transitions_by_var)
{
    int label = op.get_id();
    FactPair fact = effect.get_fact();
    int var_id = fact.var;
    has_effect_on_var[var_id] = true;
    int post_value = fact.value;

    // Determine possible values that var can have when this
    // operator is applicable.
    int pre_value = -1;
    auto pre_val_it = pre_val.find(var_id);
    if (pre_val_it != pre_val.end()) pre_value = pre_val_it->second;
    int pre_value_min, pre_value_max;
    if (pre_value == -1) {
        pre_value_min = 0;
        pre_value_max = variables[var_id].get_domain_size();
    } else {
        pre_value_min = pre_value;
        pre_value_max = pre_value + 1;
    }

    /*
      cond_effect_pre_value == x means that the effect has an
      effect condition "var == x".
      cond_effect_pre_value == -1 means no effect condition on var.
      has_other_effect_cond is true iff there exists an effect
      condition on a variable other than var.
    */
    auto effect_conditions = effect.get_conditions();
    int cond_effect_pre_value = -1;
    bool has_other_effect_cond = false;
    for (const auto [c_var, value] : effect_conditions) {
        if (c_var == var_id) {
            cond_effect_pre_value = value;
        } else {
            has_other_effect_cond = true;
        }
    }

    // Handle transitions that occur when the effect triggers.
    for (int value = pre_value_min; value < pre_value_max; ++value) {
        /*
          Only add a transition if it is possible that the effect
          triggers. We can rule out that the effect triggers if it has
          a condition on var and this condition is not satisfied.
        */
        if (cond_effect_pre_value == -1 || cond_effect_pre_value == value)
            transitions_by_var[var_id].emplace_back(value, post_value);
    }

    // Handle transitions that occur when the effect does not trigger.
    if (!effect_conditions.empty()) {
        for (int value = pre_value_min; value < pre_value_max; ++value) {
            /*
              Add self-loop if the effect might not trigger.
              If the effect has a condition on another variable, then
              it can fail to trigger no matter which value var has.
              If it only has a condition on var, then the effect
              fails to trigger if this condition is false.
            */
            if (has_other_effect_cond || value != cond_effect_pre_value)
                transitions_by_var[var_id].emplace_back(value, value);
        }
        task_has_conditional_effects = true;
    }
    mark_as_relevant(var_id, label);
}

void FTSFactory::handle_operator_precondition(
    OperatorProxy op,
    FactPair precondition,
    const vector<bool>& has_effect_on_var,
    vector<vector<Transition>>& transitions_by_var)
{
    if (const int var_id = precondition.var; !has_effect_on_var[var_id]) {
        int value = precondition.value;
        transitions_by_var[var_id].emplace_back(value, value);
        mark_as_relevant(var_id, op.get_id());
    }
}

void FTSFactory::build_transitions_for_operator(
    const VariableSpace& variables,
    OperatorProxy op)
{
    const auto& cost_function = get_cost_function(task);

    /*
      - Mark op as relevant in the transition systems corresponding
        to variables on which it has a precondition or effect.
      - Add transitions induced by op in these transition systems.
    */
    unordered_map<int, int> pre_val = compute_preconditions(op);
    const int num_variables = variables.size();
    vector<bool> has_effect_on_var(num_variables, false);
    vector<vector<Transition>> transitions_by_var(num_variables);

    for (auto effect : op.get_effects())
        handle_operator_effect(
            variables,
            op,
            effect,
            pre_val,
            has_effect_on_var,
            transitions_by_var);

    /*
      We must handle preconditions *after* effects because handling
      the effects sets has_effect_on_var.
    */
    for (FactPair precondition : op.get_preconditions())
        handle_operator_precondition(
            op,
            precondition,
            has_effect_on_var,
            transitions_by_var);

    int label = op.get_id();
    int label_cost = cost_function.get_operator_cost(op.get_id());
    for (int var_id = 0; var_id < num_variables; ++var_id) {
        if (!is_relevant(var_id, label)) {
            /*
              We do not want to add transitions of irrelevant labels here,
              since they are handled together in a separate step.
            */
            continue;
        }
        vector<Transition>& transitions = transitions_by_var[var_id];
        /*
          TODO: Our method for generating transitions is only guarantueed
          to generate sorted and unique transitions if the task has no
          conditional effects.
        */
        if (task_has_conditional_effects) {
            utils::sort_unique(transitions);
        } else {
            assert(utils::is_sorted_unique(transitions));
        }

        vector<int>& label_to_local_label =
            transition_system_data_by_var[var_id].label_to_local_label;
        vector<LocalLabelInfo>& local_label_infos =
            transition_system_data_by_var[var_id].local_label_infos;
        bool found_locally_equivalent_label_group = false;
        for (size_t local_label = 0; local_label < local_label_infos.size();
             ++local_label) {
            LocalLabelInfo& local_label_info = local_label_infos[local_label];
            const vector<Transition>& local_label_transitions =
                local_label_info.get_transitions();
            if (transitions == local_label_transitions) {
                assert(label_to_local_label[label] == -1);
                label_to_local_label[label] = local_label;
                local_label_info.add_label(label, label_cost);
                found_locally_equivalent_label_group = true;
                break;
            }
        }

        if (!found_locally_equivalent_label_group) {
            int new_local_label = local_label_infos.size();
            LabelGroup label_group = {label};
            local_label_infos.emplace_back(
                std::move(label_group),
                std::move(transitions),
                label_cost);
            assert(label_to_local_label[label] == -1);
            label_to_local_label[label] = new_local_label;
        }
    }
}

void FTSFactory::build_transitions_for_irrelevant_ops(
    const VariableProxy& variable,
    const Labels& labels)
{
    int var_id = variable.get_id();
    int num_states = variable.get_domain_size();

    // Collect all irrelevant labels for this variable.
    LabelGroup irrelevant_labels;
    int cost = INF;
    for (int label : labels) {
        if (!is_relevant(var_id, label)) {
            irrelevant_labels.push_back(label);
            cost = min(cost, labels.get_label_cost(label));
        }
    }

    TransitionSystemData& ts_data = transition_system_data_by_var[var_id];
    if (!irrelevant_labels.empty()) {
        vector<Transition> transitions;
        transitions.reserve(num_states);
        for (int state = 0; state < num_states; ++state)
            transitions.emplace_back(state, state);
        int new_local_label = ts_data.local_label_infos.size();
        for (int label : irrelevant_labels) {
            assert(ts_data.label_to_local_label[label] == -1);
            ts_data.label_to_local_label[label] = new_local_label;
        }
        ts_data.local_label_infos.emplace_back(
            std::move(irrelevant_labels),
            std::move(transitions),
            cost);
    }
}

void FTSFactory::build_transitions(const Labels& labels)
{
    const auto& [variables, operators] =
        slice<VariableSpace&, ClassicalOperatorSpace&>(task);
    /*
      - Compute all transitions of all operators for all variables, grouping
        transitions of locally equivalent labels for a given variable.
      - Computes relevant operator information as a side effect.
    */
    for (const OperatorProxy op : operators)
        build_transitions_for_operator(variables, op);

    /*
      Compute transitions of irrelevant operators for each variable only
      once and put the labels into a single label group.
    */
    for (const VariableProxy variable : variables)
        build_transitions_for_irrelevant_ops(variable, labels);
}

vector<unique_ptr<TransitionSystem>>
FTSFactory::create_transition_systems(const Labels& labels)
{
    const auto& variables = get_variables(task);

    // Create the actual TransitionSystem objects.
    const int num_variables = variables.size();

    // We reserve space for the transition systems added later by merging.
    vector<unique_ptr<TransitionSystem>> result;
    assert(num_variables >= 1);
    result.reserve(num_variables * 2 - 1);

    for (int var_id = 0; var_id < num_variables; ++var_id) {
        TransitionSystemData& ts_data = transition_system_data_by_var[var_id];
        result.push_back(
            std::make_unique<TransitionSystem>(
                ts_data.num_variables,
                std::move(ts_data.incorporated_variables),
                labels,
                std::move(ts_data.label_to_local_label),
                std::move(ts_data.local_label_infos),
                ts_data.num_states,
                std::move(ts_data.goal_states),
                ts_data.init_state));
    }

    return result;
}

vector<unique_ptr<MergeAndShrinkRepresentation>>
FTSFactory::create_mas_representations() const
{
    const auto& variables = get_variables(task);

    // Create the actual MergeAndShrinkRepresentation objects.
    const int num_variables = variables.size();

    // We reserve space for the transition systems added later by merging.
    vector<unique_ptr<MergeAndShrinkRepresentation>> result;
    assert(num_variables >= 1);
    result.reserve(num_variables * 2 - 1);

    for (int var_id = 0; var_id < num_variables; ++var_id) {
        int range = variables[var_id].get_domain_size();
        result.push_back(
            std::make_unique<MergeAndShrinkRepresentationLeaf>(var_id, range));
    }

    return result;
}

vector<unique_ptr<Distances>> FTSFactory::create_distances(
    const vector<unique_ptr<TransitionSystem>>& transition_systems) const
{
    const auto& variables = get_variables(task);

    // Create the actual Distances objects.
    const int num_variables = variables.size();

    // We reserve space for the transition systems added later by merging.
    vector<unique_ptr<Distances>> result;
    assert(num_variables >= 1);
    result.reserve(num_variables * 2 - 1);

    for (int var_id = 0; var_id < num_variables; ++var_id) {
        result.push_back(
            std::make_unique<Distances>(*transition_systems[var_id]));
    }

    return result;
}

FactoredTransitionSystem FTSFactory::create(
    const bool compute_init_distances,
    const bool compute_goal_distances,
    utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Building atomic transition systems... " << endl;
    }

    unique_ptr<Labels> labels = create_labels();

    initialize_transition_system_data(*labels);
    build_transitions(*labels);
    vector<unique_ptr<TransitionSystem>> transition_systems =
        create_transition_systems(*labels);
    vector<unique_ptr<MergeAndShrinkRepresentation>> mas_representations =
        create_mas_representations();
    vector<unique_ptr<Distances>> distances =
        create_distances(transition_systems);

    return FactoredTransitionSystem(
        std::move(labels),
        std::move(transition_systems),
        std::move(mas_representations),
        std::move(distances),
        compute_init_distances,
        compute_goal_distances,
        log);
}

FactoredTransitionSystem create_factored_transition_system(
    const AbstractTaskTuple& task,
    const bool compute_init_distances,
    const bool compute_goal_distances,
    utils::LogProxy& log)
{
    return FTSFactory(task).create(
        compute_init_distances,
        compute_goal_distances,
        log);
}
} // namespace downward::merge_and_shrink
