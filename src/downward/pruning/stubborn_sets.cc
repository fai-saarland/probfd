#include "downward/pruning/stubborn_sets.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/collections.h"

using namespace std;

namespace downward::stubborn_sets {
StubbornSets::StubbornSets(utils::Verbosity verbosity)
    : PruningMethod(verbosity)
    , num_operators(-1)
{
}

void StubbornSets::initialize(const SharedAbstractTask& task)
{
    const auto& [variables, axioms, operators, goals] = slice_shared<
        VariableSpace,
        AxiomSpace,
        ClassicalOperatorSpace,
        GoalFactList>(task);

    task_properties::verify_no_axioms(*axioms);
    task_properties::verify_no_conditional_effects(*operators);

    PruningMethod::initialize(task);

    num_operators = operators->size();
    sorted_goals =
        utils::sorted<FactPair>(task_properties::get_fact_pairs(*goals));

    compute_sorted_operators(*operators);
    compute_achievers(*variables, *operators);
}

void StubbornSets::compute_sorted_operators(
    const ClassicalOperatorSpace& operators)
{
    sorted_op_preconditions = utils::map_vector<vector<FactPair>>(
        operators,
        [](const auto& op) {
            return utils::sorted<FactPair>(
                task_properties::get_fact_pairs(op.get_preconditions()));
        });

    sorted_op_effects = utils::map_vector<vector<FactPair>>(
        operators,
        [](const auto& op) {
            return utils::sorted<FactPair>(utils::map_vector<FactPair>(
                op.get_effects(),
                [](const auto& eff) { return eff.get_fact(); }));
        });
}

void StubbornSets::compute_achievers(
    const VariableSpace& variables,
    const ClassicalOperatorSpace& operators)
{
    achievers = utils::map_vector<vector<vector<int>>>(
        variables,
        [](const VariableProxy& var) {
            return vector<vector<int>>(var.get_domain_size());
        });

    for (const OperatorProxy op : operators) {
        for (const auto effect : op.get_effects()) {
            FactPair fact = effect.get_fact();
            achievers[fact.var][fact.value].push_back(op.get_id());
        }
    }
}

void StubbornSets::prune(const State& state, vector<OperatorID>& op_ids)
{
    // Clear stubborn set from previous call.
    stubborn.assign(num_operators, false);

    compute_stubborn_set(state);

    // Now check which applicable operators are in the stubborn set.
    vector<OperatorID> remaining_op_ids;
    remaining_op_ids.reserve(op_ids.size());
    for (OperatorID op_id : op_ids) {
        if (stubborn[op_id.get_index()]) {
            remaining_op_ids.emplace_back(op_id);
        }
    }
    op_ids.swap(remaining_op_ids);
}
} // namespace downward::stubborn_sets
