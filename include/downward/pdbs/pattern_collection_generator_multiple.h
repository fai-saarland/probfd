#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "downward/pdbs/pattern_generator.h"

#include <set>
#include <unordered_set>

namespace downward::utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace downward::utils

namespace downward::pdbs {
/*
  This pattern collection generator is a general framework for computing a
  pattern collection for a given planning task. It is an abstract base class
  which must be subclasses to provide a method for computing a single pattern
  for the given task and a single goal of the task.

  The algorithm works as follows. It first stores the goals of the task in
  random order. Then, it repeatedly iterates over all goals and for each goal,
  it uses the given method for computing a single pattern. If the pattern is
  new (duplicate detection), it is kept for the final collection.

  The algorithm runs until reaching a given time limit. Another parameter allows
  exiting early if no new patterns are found for a certain time ("stagnation").
  Further parameters allow enabling blacklisting for the given pattern
  computation method after a certain time to force some diversification or to
  enable said blacklisting when stagnating.
*/
class PatternCollectionGeneratorMultiple : public PatternCollectionGenerator {
    const int max_pdb_size;
    const utils::Duration pattern_generation_max_time;
    const utils::Duration total_max_time;
    const utils::Duration stagnation_limit;
    const utils::Duration blacklisting_start_duration;
    const bool enable_blacklist_on_stagnation;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    const int random_seed;

    // Variables used in the main loop.
    int remaining_collection_size;
    bool blacklisting;
    utils::Duration time_point_of_last_new_pattern;

    void check_blacklist_trigger_timer(const utils::CountdownTimer& timer);
    std::unordered_set<int>
    get_blacklisted_variables(std::vector<int>& non_goal_variables);
    void handle_generated_pattern(
        PatternInformation&& pattern_info,
        std::set<Pattern>& generated_patterns,
        std::shared_ptr<PDBCollection>& generated_pdbs,
        const utils::CountdownTimer& timer);
    bool collection_size_limit_reached() const;
    bool time_limit_reached(const utils::CountdownTimer& timer) const;
    bool check_for_stagnation(const utils::CountdownTimer& timer);
    virtual std::string id() const = 0;
    virtual void initialize(const SharedAbstractTask& task) = 0;
    virtual PatternInformation compute_pattern(
        int max_pdb_size,
        utils::Duration max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const SharedAbstractTask& task,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) = 0;
    std::string name() const override;
    PatternCollectionInformation
    compute_patterns(const SharedAbstractTask& task) override;

public:
    PatternCollectionGeneratorMultiple(
        int max_pdb_size,
        int max_collection_size,
        utils::Duration pattern_generation_max_time,
        utils::Duration total_max_time,
        utils::Duration stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        int random_seed,
        utils::Verbosity verbosity);
};

} // namespace downward::pdbs

#endif
