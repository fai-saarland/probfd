#include "downward/potentials/diverse_potential_heuristics.h"

#include "downward/potentials/potential_function.h"
#include "downward/potentials/potential_max_heuristic.h"
#include "downward/potentials/util.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/timer.h"

using namespace std;

namespace downward::potentials {

DiversePotentialHeuristics::DiversePotentialHeuristics(
    int num_samples,
    int max_num_heuristics,
    double max_potential,
    lp::LPSolverType lpsolver,
    const shared_ptr<AbstractTask>& transform,
    int random_seed,
    utils::Verbosity verbosity)
    : optimizer(transform, lpsolver, max_potential)
    , max_num_heuristics(max_num_heuristics)
    , num_samples(num_samples)
    , rng(utils::get_rng(random_seed))
    , log(utils::get_log_for_verbosity(verbosity))
{
}

DiversePotentialHeuristics::~DiversePotentialHeuristics() = default;

SamplesToFunctionsMap
DiversePotentialHeuristics::filter_samples_and_compute_functions(
    const vector<State>& samples)
{
    utils::Timer filtering_timer;
    utils::HashSet<State> dead_ends;
    int num_duplicates = 0;
    int num_dead_ends = 0;
    SamplesToFunctionsMap samples_to_functions;
    for (const State& sample : samples) {
        // Skipping duplicates is not necessary, but saves LP evaluations.
        if (samples_to_functions.count(sample) || dead_ends.count(sample)) {
            ++num_duplicates;
            continue;
        }
        optimizer.optimize_for_state(sample);
        if (optimizer.has_optimal_solution()) {
            samples_to_functions[sample] = optimizer.get_potential_function();
        } else {
            dead_ends.insert(sample);
            ++num_dead_ends;
        }
    }
    if (log.is_at_least_normal()) {
        log << "Time for filtering dead ends: " << filtering_timer << endl;
        log << "Duplicate samples: " << num_duplicates << endl;
        log << "Dead end samples: " << num_dead_ends << endl;
        log << "Unique non-dead-end samples: " << samples_to_functions.size()
            << endl;
    }
    assert(
        num_duplicates + num_dead_ends + samples_to_functions.size() ==
        samples.size());
    return samples_to_functions;
}

void DiversePotentialHeuristics::remove_covered_samples(
    const PotentialFunction& chosen_function,
    SamplesToFunctionsMap& samples_to_functions) const
{
    for (auto it = samples_to_functions.begin();
         it != samples_to_functions.end();) {
        const State& sample = it->first;
        const PotentialFunction& sample_function = *it->second;
        int max_h = sample_function.get_value(sample);
        int h = chosen_function.get_value(sample);
        assert(h <= max_h);
        // TODO: Count as covered if max_h <= 0.
        if (h == max_h) {
            it = samples_to_functions.erase(it);
        } else {
            ++it;
        }
    }
}

unique_ptr<PotentialFunction>
DiversePotentialHeuristics::find_function_and_remove_covered_samples(
    SamplesToFunctionsMap& samples_to_functions)
{
    vector<State> uncovered_samples;
    for (auto& sample_and_function : samples_to_functions) {
        const State& state = sample_and_function.first;
        uncovered_samples.push_back(state);
    }
    optimizer.optimize_for_samples(uncovered_samples);
    unique_ptr<PotentialFunction> function = optimizer.get_potential_function();
    size_t last_num_samples = samples_to_functions.size();
    remove_covered_samples(*function, samples_to_functions);
    if (samples_to_functions.size() == last_num_samples) {
        if (log.is_at_least_verbose()) {
            log << "No sample removed -> Use arbitrary precomputed function."
                << endl;
        }
        function = std::move(samples_to_functions.begin()->second);
        // The move operation invalidated the entry, remove it.
        samples_to_functions.erase(samples_to_functions.begin());
        remove_covered_samples(*function, samples_to_functions);
    }
    if (log.is_at_least_verbose()) {
        log << "Removed " << last_num_samples - samples_to_functions.size()
            << " samples. " << samples_to_functions.size() << " remaining."
            << endl;
    }
    return function;
}

void DiversePotentialHeuristics::cover_samples(
    SamplesToFunctionsMap& samples_to_functions)
{
    utils::Timer covering_timer;
    while (!samples_to_functions.empty() &&
           static_cast<int>(diverse_functions.size()) < max_num_heuristics) {
        if (log.is_at_least_verbose()) {
            log << "Find heuristic #" << diverse_functions.size() + 1 << endl;
        }
        diverse_functions.push_back(
            find_function_and_remove_covered_samples(samples_to_functions));
    }
    if (log.is_at_least_normal()) {
        log << "Time for covering samples: " << covering_timer << endl;
    }
}

vector<unique_ptr<PotentialFunction>>
DiversePotentialHeuristics::find_functions()
{
    assert(diverse_functions.empty());
    utils::Timer init_timer;

    // Sample states.
    vector<State> samples =
        sample_without_dead_end_detection(optimizer, num_samples, *rng);

    // Filter dead end samples.
    SamplesToFunctionsMap samples_to_functions =
        filter_samples_and_compute_functions(samples);

    // Iteratively cover samples.
    cover_samples(samples_to_functions);

    if (log.is_at_least_normal()) {
        log << "Potential heuristics: " << diverse_functions.size() << endl;
        log << "Initialization of potential heuristics: " << init_timer << endl;
    }

    return std::move(diverse_functions);
}

} // namespace potentials
