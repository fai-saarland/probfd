#include "downward/landmarks/landmark_factory_rpg_exhaust.h"

#include "downward/landmarks/landmark.h"

#include "downward/task_proxy.h"

#include "downward/utils/logging.h"

#include <vector>

using namespace std;

namespace downward::landmarks {
/* Problem: We don't get any orders here. (All we have is the reasonable orders
   that are inferred later.) It's thus best to combine this landmark generation
   method with others, don't use it by itself. */

LandmarkFactoryRpgExhaust::LandmarkFactoryRpgExhaust(
    bool use_unary_relaxation,
    utils::Verbosity verbosity)
    : LandmarkFactoryRelaxation(verbosity)
    , use_unary_relaxation(use_unary_relaxation)
{
}

void LandmarkFactoryRpgExhaust::generate_relaxed_landmarks(
    const shared_ptr<AbstractTask>& task,
    Exploration& exploration)
{
    TaskProxy task_proxy(*task);
    if (log.is_at_least_normal()) {
        log << "Generating landmarks by testing all facts with RPG method"
            << endl;
    }

    // insert goal landmarks and mark them as goals
    for (FactProxy goal : task_proxy.get_goals()) {
        Landmark landmark({goal.get_pair()}, false, false, true);
        lm_graph->add_landmark(std::move(landmark));
    }
    // test all other possible facts
    State initial_state = task_proxy.get_initial_state();
    for (VariableProxy var : task_proxy.get_variables()) {
        for (int value = 0; value < var.get_domain_size(); ++value) {
            const FactPair lm(var.get_id(), value);
            if (!lm_graph->contains_simple_landmark(lm)) {
                Landmark landmark({lm}, false, false);
                if (!relaxed_task_solvable(
                        task_proxy,
                        exploration,
                        landmark,
                        use_unary_relaxation)) {
                    lm_graph->add_landmark(std::move(landmark));
                }
            }
        }
    }
}

bool LandmarkFactoryRpgExhaust::supports_conditional_effects() const
{
    return false;
}

} // namespace landmarks
