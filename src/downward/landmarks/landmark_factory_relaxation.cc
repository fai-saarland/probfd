#include "downward/landmarks/landmark_factory_relaxation.h"

#include "downward/landmarks/exploration.h"
#include "downward/landmarks/landmark.h"

#include "downward/task_utils/task_properties.h"

using namespace std;

namespace downward::landmarks {
LandmarkFactoryRelaxation::LandmarkFactoryRelaxation(utils::Verbosity verbosity)
    : LandmarkFactory(verbosity)
{
}

void LandmarkFactoryRelaxation::generate_landmarks(
    const shared_ptr<AbstractTask>& task)
{
    TaskProxy task_proxy(*task);
    Exploration exploration(task_proxy, log);
    generate_relaxed_landmarks(task, exploration);
    postprocess(task_proxy, exploration);
}

void LandmarkFactoryRelaxation::postprocess(
    const TaskProxy& task_proxy,
    Exploration& exploration)
{
    lm_graph->set_landmark_ids();
    calc_achievers(task_proxy, exploration);
}

void LandmarkFactoryRelaxation::calc_achievers(
    const TaskProxy& task_proxy,
    Exploration& exploration)
{
    assert(!achievers_calculated);
    AxiomsProxy axioms = task_proxy.get_axioms();
    for (auto& lm_node : lm_graph->get_nodes()) {
        Landmark& landmark = lm_node->get_landmark();
        for (const FactPair& lm_fact : landmark.facts) {
            const vector<int>& ops = get_operators_including_eff(lm_fact);
            landmark.possible_achievers.insert(ops.begin(), ops.end());

            if (axioms.is_derived(lm_fact.var)) landmark.is_derived = true;
        }

        vector<vector<bool>> reached =
            exploration.compute_relaxed_reachability(landmark.facts, false);

        for (int op_or_axom_id : landmark.possible_achievers) {
            AxiomOrOperatorProxy op =
                get_operator_or_axiom(task_proxy, op_or_axom_id);

            if (possibly_reaches_lm(op, reached, landmark)) {
                landmark.first_achievers.insert(op_or_axom_id);
            }
        }
    }
    achievers_calculated = true;
}

bool LandmarkFactoryRelaxation::relaxed_task_solvable(
    const TaskProxy& task_proxy,
    Exploration& exploration,
    const Landmark& exclude,
    const bool use_unary_relaxation) const
{
    vector<vector<bool>> reached = exploration.compute_relaxed_reachability(
        exclude.facts,
        use_unary_relaxation);

    for (FactProxy goal : task_proxy.get_goals()) {
        if (!reached[goal.get_variable().get_id()][goal.get_value()]) {
            return false;
        }
    }
    return true;
}

} // namespace landmarks
