#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"

#include "utils/logging.h"

#include <cstdlib>
#include <memory>
#include <set>
#include <vector>

namespace options {
class Options;
}

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

class State;

namespace sampling {
class RandomWalkSampler;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;
class IncrementalPPDBs;

// Implementation of the pattern generation algorithm by Haslum et al.
class PatternCollectionGeneratorHillclimbing
    : public PatternCollectionGenerator {
    struct Statistics : public utils::Printable {
        unsigned long long int num_iterations;
        unsigned long long int generated_patterns;
        unsigned long long int rejected_patterns;
        unsigned long long int max_pdb_size;

        double hillclimbing_time;

        Statistics(
            unsigned long long int num_iterations,
            unsigned long long int generated_patterns,
            unsigned long long int rejected_patterns,
            unsigned long long int max_pdb_size,
            double hillclimbing_time)
            : num_iterations(num_iterations)
            , generated_patterns(generated_patterns)
            , rejected_patterns(rejected_patterns)
            , max_pdb_size(max_pdb_size)
            , hillclimbing_time(hillclimbing_time)
        {
        }

        void print(std::ostream& out) const override;
    };

    const utils::Verbosity verbosity;

    std::shared_ptr<Statistics> statistics_;

    std::shared_ptr<PatternCollectionGenerator> initial_generator;
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory;

    // maximum number of states for each pdb
    const int pdb_max_size;
    // maximum added size of all pdbs
    const int collection_max_size;
    const int num_samples;
    // minimal improvement required for hill climbing to continue search
    const int min_improvement;
    const double max_time;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    // for stats only
    int num_rejected;

    /*
      For the given PDB, all possible extensions of its pattern by one
      relevant variable are considered as candidate patterns. If the candidate
      pattern has not been previously considered (not contained in
      generated_patterns) and if building a PDB for it does not surpass the
      size limit, then the PDB is built and added to candidate_pdbs.

      The method returns the size of the largest PDB added to candidate_pdbs.
    */
    int generate_candidate_pdbs(
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        utils::CountdownTimer& hill_climbing_timer,
        const std::vector<std::vector<int>>& relevant_neighbours,
        const ProbabilisticPatternDatabase& pdb,
        std::set<Pattern>& generated_patterns,
        PPDBCollection& candidate_pdbs);

    /*
      Performs num_samples random walks with a length (different for each
      random walk) chosen according to a binomial distribution with
      n = 4 * solution depth estimate and p = 0.5, starting from the initial
      state. In each step of a random walk, a random operator is taken and
      applied to the current state. If a dead end is reached or no more
      operators are applicable, the walk starts over again from the initial
      state. At the end of each random walk, the last state visited is taken as
      a sample state, thus totalling exactly num_samples of sample states.
    */
    void sample_states(
        utils::CountdownTimer& hill_climbing_timer,
        IncrementalPPDBs& current_pdbs,
        const sampling::RandomWalkSampler& sampler,
        value_t init_h,
        std::vector<State>& samples);

    /*
      Searches for the best improving pdb in candidate_pdbs according to the
      counting approximation and the given samples. Returns the improvement and
      the index of the best pdb in candidate_pdbs.
    */
    std::pair<int, int> find_best_improving_pdb(
        utils::CountdownTimer& hill_climbing_timer,
        IncrementalPPDBs& current_pdbs,
        const std::vector<State>& samples,
        const std::vector<EvaluationResult>& samples_h_values,
        PPDBCollection& candidate_pdbs);

    /*
      Returns true iff the h-value of the new pattern (from pdb) plus the
      h-value of all pattern cliques from the current pattern
      collection heuristic if the new pattern was added to it is greater than
      the h-value of the current pattern collection.
    */
    bool is_heuristic_improved(
        const ProbabilisticPatternDatabase& pdb,
        const State& sample,
        EvaluationResult h_collection,
        const PPDBCollection& pdbs,
        const std::vector<PatternSubCollection>& pattern_subcollections,
        const IncrementalPPDBs& current_pdbs);

    /*
      This is the core algorithm of this class. The initial PDB collection
      consists of one PDB for each goal variable. For each PDB of this initial
      collection, the set of candidate PDBs are added (see
      generate_candidate_pdbs) to the set of initial candidate PDBs.

      The main loop of the search computes a set of sample states (see
      sample_states) and uses this set to evaluate the set of all candidate PDBs
      (see find_best_improving_pdb, using the "counting approximation"). If the
      improvement obtained through adding the best PDB to the current heuristic
      is smaller than the minimal required improvement, the search is stopped.
      Otherwise, the best PDB is added to the heuristic and the candidate PDBs
      for this best PDB are computed (see generate_candidate_pdbs) and used for
      the next iteration.

      This method uses a set to store all patterns that are generated as
      candidate patterns in their "normal form" for duplicate detection.
      Futhermore, a vector stores the PDBs corresponding to the candidate
      patterns if its size does not surpass the user-specified size limit.
      Storing the PDBs has the only purpose to avoid re-computation of the same
      PDBs. This is quite a large time gain, but may use a lot of memory.
    */
    void hill_climbing(
        const ProbabilisticTask* task,
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        IncrementalPPDBs& current_pdbs);

public:
    explicit PatternCollectionGeneratorHillclimbing(
        const options::Options& opts);
    ~PatternCollectionGeneratorHillclimbing();

    /*
      Runs the hill climbing algorithm. Note that the
      initial pattern collection (consisting of exactly one PDB for each goal
      variable) may break the maximum collection size limit, if the latter is
      set too small or if there are many goal variables with a large domain.
    */
    PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

    std::shared_ptr<utils::Printable> get_report() const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H__