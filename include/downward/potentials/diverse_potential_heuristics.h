#ifndef POTENTIALS_DIVERSE_POTENTIAL_HEURISTICS_H
#define POTENTIALS_DIVERSE_POTENTIAL_HEURISTICS_H

#include "downward/potentials/potential_optimizer.h"

#include "downward/utils/logging.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::potentials {
using SamplesToFunctionsMap =
    utils::HashMap<State, std::unique_ptr<PotentialFunction>>;

/*
  Factory class that finds diverse potential functions.
*/
class DiversePotentialHeuristics {
    PotentialOptimizer optimizer;
    // TODO: Remove max_num_heuristics and control number of heuristics
    // with num_samples parameter?
    const int max_num_heuristics;
    const int num_samples;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    utils::LogProxy log;
    std::vector<std::unique_ptr<PotentialFunction>> diverse_functions;

    /* Filter dead end samples and duplicates. Store potential heuristics
       for remaining samples. */
    SamplesToFunctionsMap
    filter_samples_and_compute_functions(const std::vector<State>& samples);

    // Remove all samples for which the function achieves maximal values.
    void remove_covered_samples(
        const PotentialFunction& chosen_function,
        SamplesToFunctionsMap& samples_to_functions) const;

    /* Return potential function optimized for remaining samples or a
       precomputed heuristic if the former does not cover additional samples. */
    std::unique_ptr<PotentialFunction> find_function_and_remove_covered_samples(
        SamplesToFunctionsMap& samples_to_functions);

    /* Iteratively try to find potential functions that achieve maximal values
       for as many samples as possible. */
    void cover_samples(SamplesToFunctionsMap& samples_to_functions);

public:
    DiversePotentialHeuristics(
        int num_samples,
        int max_num_heuristics,
        double max_potential,
        lp::LPSolverType lpsolver,
        const std::shared_ptr<AbstractTask>& transform,
        int random_seed,
        utils::Verbosity verbosity);
    
    ~DiversePotentialHeuristics();

    // Sample states, then cover them.
    std::vector<std::unique_ptr<PotentialFunction>> find_functions();
};
} // namespace potentials
#endif
