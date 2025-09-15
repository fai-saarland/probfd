#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/value_type.h"

#include <cstdint>
#include <memory>
#include <set>
#include <vector>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace downward::utils

namespace downward::dynamic_bitset {
template <typename>
class DynamicBitset;
}

namespace probfd::sampling {
class RandomWalkSampler;
}

namespace probfd::pdbs {
class SubCollectionFinderFactory;
}

namespace probfd::pdbs {

// Implementation of the pattern generation algorithm by Haslum et al.
class PatternCollectionGeneratorHillclimbing final
    : public PatternCollectionGenerator {
    using DynamicBitset = downward::dynamic_bitset::DynamicBitset<uint64_t>;

    struct Sample;
    class IncrementalPPDBs;

    std::shared_ptr<PatternCollectionGenerator> initial_generator_;
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory_;

    // maximum number of states for each pdb
    const int pdb_max_size_;
    // maximum added size of all pdbs
    const int collection_max_size_;
    const int num_samples_;
    // minimal improvement required for hill climbing to continue search
    const int min_improvement_;
    const downward::utils::Duration max_time_;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

    // maximum size of the PDB search space
    int remaining_states_;

    // for stats only
    int num_rejected_;

    /*
      For the given PDB, all possible extensions of its pattern by one
      relevant variable are considered as candidate patterns. If the candidate
      pattern has not been previously considered (not contained in
      generated_patterns) and if building a PDB for it does not surpass the
      size limit, then the PDB is built and added to candidate_pdbs.

      The method returns the size of the largest PDB added to candidate_pdbs.
    */
    unsigned int generate_candidate_pdbs(
        const SharedProbabilisticTask& task,
        const downward::State& initial_state,
        downward::utils::CountdownTimer& hill_climbing_timer,
        const std::vector<std::vector<int>>& relevant_neighbours,
        const ProbabilityAwarePatternDatabase& pdb,
        std::set<DynamicBitset>& generated_patterns,
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
        const downward::State& initial_state,
        const downward::utils::CountdownTimer& hill_climbing_timer,
        IncrementalPPDBs& current_pdbs,
        const sampling::RandomWalkSampler& sampler,
        value_t init_h,
        value_t cost_lower_bound,
        value_t termination_cost,
        std::vector<Sample>& samples) const;

    /*
      Searches for the best improving pdb in candidate_pdbs according to the
      counting approximation and the given samples. Returns the improvement and
      the index of the best pdb in candidate_pdbs.
    */
    std::pair<int, int> find_best_improving_pdb(
        downward::utils::CountdownTimer& hill_climbing_timer,
        IncrementalPPDBs& current_pdbs,
        const std::vector<Sample>& samples,
        PPDBCollection& candidate_pdbs,
        value_t termination_cost) const;

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
        const SharedProbabilisticTask& task,
        const downward::State& initial_state,
        IncrementalPPDBs& current_pdbs,
        value_t cost_lower_bound);

public:
    explicit PatternCollectionGeneratorHillclimbing(
        std::shared_ptr<PatternCollectionGenerator> initial_generator,
        std::shared_ptr<SubCollectionFinderFactory>
            subcollection_finder_factory,
        int pdb_max_size,
        int collection_max_size,
        int num_samples,
        int min_improvement,
        downward::utils::Duration max_time,
        int search_space_max_size,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng,
        downward::utils::Verbosity verbosity);

    ~PatternCollectionGeneratorHillclimbing() override;

    /*
      Runs the hill climbing algorithm. Note that the
      initial pattern collection (consisting of exactly one PDB for each goal
      variable) may break the maximum collection size limit, if the latter is
      set too small or if there are many goal variables with a large domain.
    */
    PatternCollectionInformation
    generate(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
