#include "downward/tasks/domain_abstracted_task_factory.h"

#include "downward/tasks/domain_abstracted_task.h"

#include "downward/transformations/domain_abstraction.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/collections.h"
#include "probfd/tasks/domain_abstracted_task.h"

#include <sstream>
#include <string>
#include <unordered_set>

using namespace std;

namespace downward::extra_tasks {

class DomainAbstractedTaskFactory {
    vector<int> domain_size;
    vector<int> initial_state_values;
    vector<FactPair> goal_facts;
    vector<vector<string>> fact_names;
    vector<vector<int>> value_map;
    SharedAbstractTask task;

    void initialize(const AbstractTaskTuple& parent);
    void combine_values(int var, const ValueGroups& groups);
    string get_combined_fact_name(int var, const ValueGroup& values) const;

public:
    DomainAbstractedTaskFactory(
        const SharedAbstractTask& parent,
        const VarToGroups& value_groups);
    ~DomainAbstractedTaskFactory() = default;

    SharedAbstractTask get_task() &&;
};

DomainAbstractedTaskFactory::DomainAbstractedTaskFactory(
    const SharedAbstractTask& parent,
    const VarToGroups& value_groups)
{
    const auto& [variables, axioms, operators, goals, init_values, cost_function] =
        parent;

    if (task_properties::has_axioms(*axioms)) {
        ABORT("DomainAbstractedTask doesn't support axioms.");
    }

    if (task_properties::has_conditional_effects(*operators)) {
        ABORT("DomainAbstractedTask doesn't support conditional effects.");
    }

    initialize(to_refs(parent));

    for (const auto& pair : value_groups) {
        int var = pair.first;
        const ValueGroups& groups = pair.second;
        assert(utils::in_bounds(var, domain_size));
        for (const ValueGroup& group : groups) {
            for (int value : group) {
                (void)value;
                assert(0 <= value && value < domain_size[var]);
            }
        }
        combine_values(var, groups);
    }

    // Apply domain abstraction to initial state.
    for (size_t var_id = 0; var_id < initial_state_values.size(); ++var_id) {
        initial_state_values[var_id] =
            value_map[var_id][initial_state_values[var_id]];
    }

    // Apply domain abstraction to goals.
    for (FactPair& goal : goal_facts) {
        goal.value = value_map[goal.var][goal.value];
    }

    const auto domain_abstraction =
        std::make_shared<DomainAbstraction>(value_map);

    task = std::forward_as_tuple(
        std::make_unique<DomainAbstractedVariableSpace>(
            variables,
            std::move(domain_size),
            std::move(fact_names),
            domain_abstraction),
        axioms,
        std::make_unique<DomainAbstractedOperatorSpace>(
            operators,
            domain_abstraction),
        std::make_unique<DomainAbstractedGoal>(std::move(goal_facts)),
        std::make_unique<DomainAbstractedInitialStateValues>(
            initial_state_values),
        cost_function);
}

void DomainAbstractedTaskFactory::initialize(const AbstractTaskTuple& parent)
{
    const auto& [variables, init_values] =
        slice<VariableSpace&, InitialStateValues&>(parent);

    int num_vars = variables.get_num_variables();
    domain_size.resize(num_vars);
    initial_state_values = init_values.get_initial_state_values();
    value_map.resize(num_vars);
    fact_names.resize(num_vars);
    for (int var = 0; var < num_vars; ++var) {
        int num_values = variables.get_variable_domain_size(var);
        domain_size[var] = num_values;
        value_map[var].resize(num_values);
        fact_names[var].resize(num_values);
        for (int value = 0; value < num_values; ++value) {
            value_map[var][value] = value;
            fact_names[var][value] =
                variables.get_fact_name(FactPair(var, value));
        }
    }
}

string DomainAbstractedTaskFactory::get_combined_fact_name(
    int var,
    const ValueGroup& values) const
{
    ostringstream name;
    string sep;
    for (int value : values) {
        name << sep << fact_names[var][value];
        sep = " OR ";
    }
    return name.str();
}

void DomainAbstractedTaskFactory::combine_values(
    int var,
    const ValueGroups& groups)
{
    vector<string> combined_fact_names;
    unordered_set<int> groups_union;
    int num_merged_values = 0;
    (void)num_merged_values;
    for (const ValueGroup& group : groups) {
        combined_fact_names.push_back(get_combined_fact_name(var, group));
        groups_union.insert(group.begin(), group.end());
        num_merged_values += group.size();
    }
    assert(static_cast<int>(groups_union.size()) == num_merged_values);

    int next_free_pos = 0;

    // Move all facts that are not part of groups to the front.
    for (int before = 0; before < domain_size[var]; ++before) {
        if (groups_union.count(before) == 0) {
            value_map[var][before] = next_free_pos;
            fact_names[var][next_free_pos] = std::move(fact_names[var][before]);
            ++next_free_pos;
        }
    }
    int num_single_values = next_free_pos;
    assert(num_single_values + num_merged_values == domain_size[var]);

    // Add new facts for merged groups.
    for (size_t group_id = 0; group_id < groups.size(); ++group_id) {
        const ValueGroup& group = groups[group_id];
        for (int before : group) { value_map[var][before] = next_free_pos; }
        assert(utils::in_bounds(next_free_pos, fact_names[var]));
        fact_names[var][next_free_pos] =
            std::move(combined_fact_names[group_id]);
        ++next_free_pos;
    }
    int new_domain_size = num_single_values + static_cast<int>(groups.size());
    assert(next_free_pos == new_domain_size);

    // Update domain size.
    fact_names[var].resize(new_domain_size);
    domain_size[var] = new_domain_size;
}

SharedAbstractTask DomainAbstractedTaskFactory::get_task() &&
{
    return std::move(task);
}

SharedAbstractTask build_domain_abstracted_task(
    const SharedAbstractTask& parent,
    const VarToGroups& value_groups)
{
    return DomainAbstractedTaskFactory(parent, value_groups).get_task();
}
} // namespace downward::extra_tasks
