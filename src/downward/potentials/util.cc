#include "downward/potentials/util.h"

#include "downward/potentials/potential_function.h"
#include "downward/potentials/potential_optimizer.h"

#include "downward/task_utils/sampling.h"

#include "downward/heuristic.h"

using namespace std;

namespace downward::potentials {
vector<State> sample_without_dead_end_detection(
    PotentialOptimizer& optimizer,
    int num_samples,
    utils::RandomNumberGenerator& rng)
{
    const shared_ptr<AbstractTask> task = optimizer.get_task();
    State initial_state = task->get_initial_state();
    optimizer.optimize_for_state(initial_state);
    int init_h = optimizer.get_potential_function()->get_value(initial_state);
    sampling::RandomWalkSampler sampler(*task, rng);
    vector<State> samples;
    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        samples.push_back(sampler.sample_state(init_h));
    }
    return samples;
}

} // namespace potentials
