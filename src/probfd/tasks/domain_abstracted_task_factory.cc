#include "probfd/tasks/domain_abstracted_task_factory.h"

#include "probfd/tasks/domain_abstracted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/task_utils/task_properties.h"
#include "downward/tasks/domain_abstracted_task.h"

#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#include <cassert>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace std;

using namespace downward;

namespace probfd::extra_tasks {

namespace {

class DomainAbstractedTaskFactory {
    vector<int> domain_size;
    vector<int> initial_state_values;
    vector<FactPair> goal_facts;
    vector<vector<string>> fact_names;
    vector<vector<int>> value_map;
    SharedProbabilisticTask task;

    void combine_values(int var, const ValueGroups& groups);
    [[nodiscard]]
    string get_combined_fact_name(int var, const ValueGroup& values) const;

public:
    DomainAbstractedTaskFactory(
        const SharedProbabilisticTask& parent,
        const VarToGroups& value_groups);
    ~DomainAbstractedTaskFactory() = default;

    [[nodiscard]]
    SharedProbabilisticTask get_task() const;
};

DomainAbstractedTaskFactory::DomainAbstractedTaskFactory(
    const SharedProbabilisticTask& parent,
    const VarToGroups& value_groups)
{
    const auto& [variables, axioms, operators, goals, init_vals, cost_function, termination_costs] =
        to_refs(parent);

    if (::task_properties::has_axioms(axioms)) {
        ABORT("DomainAbstractedTask doesn't support axioms.");
    }
    if (task_properties::has_conditional_effects(operators)) {
        ABORT("DomainAbstractedTask doesn't support conditional effects.");
    }

    int num_vars = variables.get_num_variables();
    domain_size.resize(num_vars);
    initial_state_values = init_vals.get_initial_state_values();
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

    auto domain_abstraction =
        make_shared<DomainAbstraction>(std::move(value_map));

    task = replace(
        parent,
        std::make_shared<downward::extra_tasks::DomainAbstractedVariableSpace>(
            get_shared_variables(parent),
            std::move(domain_size),
            std::move(fact_names),
            domain_abstraction),
        std::make_shared<DomainAbstractedProbabilisticOperatorSpace>(
            get_shared_operators(parent),
            domain_abstraction),
        std::make_shared<downward::extra_tasks::DomainAbstractedGoal>(
            std::move(goal_facts)),
        std::make_shared<
            downward::extra_tasks::DomainAbstractedInitialStateValues>(
            std::move(initial_state_values)));
}

string DomainAbstractedTaskFactory::get_combined_fact_name(
    int var,
    const ValueGroup& values) const
{
    auto get_fact_name = [&names = fact_names[var]](int value) {
        return names[value];
    };

    return std::format(
        "{{{:n}}}",
        values | std::views::transform(get_fact_name));
}

void DomainAbstractedTaskFactory::combine_values(
    int var,
    const ValueGroups& groups)
{
    vector<string> combined_fact_names;
    unordered_set<int> groups_union;

#ifndef NDEBUG
    int num_merged_values = 0;
#endif

    for (const ValueGroup& group : groups) {
        combined_fact_names.push_back(get_combined_fact_name(var, group));
        groups_union.insert_range(group);

#ifndef NDEBUG
        num_merged_values += group.size();
#endif
    }

    assert(static_cast<int>(groups_union.size()) == num_merged_values);

    int next_free_pos = 0;

    // Move all facts that are not part of groups to the front.
    for (int before = 0; before < domain_size[var]; ++before) {
        if (!groups_union.contains(before)) {
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
        for (const int before : group) {
            value_map[var][before] = next_free_pos;
        }
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

SharedProbabilisticTask DomainAbstractedTaskFactory::get_task() const
{
    return task;
}

} // namespace

SharedProbabilisticTask build_domain_abstracted_task(
    const SharedProbabilisticTask& parent,
    const VarToGroups& value_groups)
{
    return DomainAbstractedTaskFactory(parent, value_groups).get_task();
}

} // namespace probfd::extra_tasks
