#include "downward/landmarks/landmark_factory_relaxation.h"

#include "downward/landmarks/exploration.h"
#include "downward/landmarks/landmark.h"

#include "downward/task_utils/task_properties.h"

#include "downward/axiom_utils.h"
#include "downward/initial_state_values.h"

using namespace std;

namespace downward::landmarks {
LandmarkFactoryRelaxation::LandmarkFactoryRelaxation(utils::Verbosity verbosity)
    : LandmarkFactory(verbosity)
{
}

void LandmarkFactoryRelaxation::generate_landmarks(
    const SharedAbstractTask& task)
{
    const auto& [variables, axioms, operators] = to_refs(
        slice_shared<VariableSpace, AxiomSpace, ClassicalOperatorSpace>(task));
    Exploration exploration(variables, axioms, operators, log);
    generate_relaxed_landmarks(task, exploration);
    postprocess(to_refs(task), exploration);
}

void LandmarkFactoryRelaxation::postprocess(
    const AbstractTaskTuple& task,
    Exploration& exploration)
{
    lm_graph->set_landmark_ids();
    calc_achievers(task, exploration);
}

void LandmarkFactoryRelaxation::calc_achievers(
    const AbstractTaskTuple& task,
    Exploration& exploration)
{
    const auto& [axioms, operators, init_vals] =
        slice<AxiomSpace&, ClassicalOperatorSpace&, InitialStateValues&>(task);

    assert(!achievers_calculated);

    for (auto& lm_node : lm_graph->get_nodes()) {
        Landmark& landmark = lm_node->get_landmark();
        for (const FactPair& lm_fact : landmark.facts) {
            const vector<int>& ops = get_operators_including_eff(lm_fact);
            landmark.possible_achievers.insert(ops.begin(), ops.end());

            if (axioms.is_derived(lm_fact.var)) landmark.is_derived = true;
        }

        vector<vector<bool>> reached = exploration.compute_relaxed_reachability(
            landmark.facts,
            init_vals.get_initial_state(),
            false);

        for (int op_or_axom_id : landmark.possible_achievers) {
            auto op = get_operator_or_axiom(axioms, operators, op_or_axom_id);

            if (possibly_reaches_lm(op, reached, landmark)) {
                landmark.first_achievers.insert(op_or_axom_id);
            }
        }
    }
    achievers_calculated = true;
}

bool LandmarkFactoryRelaxation::relaxed_task_solvable(
    const GoalFactList& goals,
    const State& state,
    Exploration& exploration,
    const Landmark& exclude,
    const bool use_unary_relaxation) const
{
    vector<vector<bool>> reached = exploration.compute_relaxed_reachability(
        exclude.facts,
        state,
        use_unary_relaxation);

    for (const auto [var, value] : goals) {
        if (!reached[var][value]) { return false; }
    }
    return true;
}

} // namespace downward::landmarks
