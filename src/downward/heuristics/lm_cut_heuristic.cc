#include "downward/heuristics/lm_cut_heuristic.h"

#include "downward/heuristics/lm_cut_landmarks.h"

#include "downward/state.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"

#include "downward/task_transformation.h"

#include <iostream>

using namespace std;

namespace downward::lm_cut_heuristic {
LandmarkCutHeuristic::LandmarkCutHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
    , landmark_generator(std::make_unique<LandmarkCutLandmarks>(transformed_task))
{
    if (log.is_at_least_normal()) {
        log << "Initializing landmark cut heuristic..." << endl;
    }
}

LandmarkCutHeuristic::LandmarkCutHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

LandmarkCutHeuristic::~LandmarkCutHeuristic() = default;

int LandmarkCutHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int total_cost = 0;
    bool dead_end = landmark_generator->compute_landmarks(
        state,
        [&total_cost](int cut_cost) { total_cost += cut_cost; },
        nullptr);

    if (dead_end) return DEAD_END;
    return total_cost;
}

} // namespace lm_cut_heuristic
