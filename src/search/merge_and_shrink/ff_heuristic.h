#ifndef MERGE_AND_SHRINK_FF_HEURISTIC_H
#define MERGE_AND_SHRINK_FF_HEURISTIC_H

#include "additive_heuristic.h"
#include "../utils/rng.h"

#include <vector>

/*
  TODO: In a better world, this should not derive from
        AdditiveHeuristic. Rather, the common parts should be
        implemented in a common base class. That refactoring could be
        made at the same time at which we also unify this with the
        other relaxation heuristics and the additional FF heuristic
        implementation in the landmark code.
*/

namespace merge_and_shrink {

class FFHeuristic : public AdditiveHeuristic {
  enum TieBreaking {
    ARBITRARY = 0,
    RANDOM = 1,
  };
  const TieBreaking c_tiebreaking;
  utils::RandomNumberGenerator m_rng;
    // Relaxed plans are represented as a set of operators implemented
    // as a bit vector.
    typedef std::vector<bool> RelaxedPlan;
    RelaxedPlan relaxed_plan;
    void mark_preferred_operators_and_relaxed_plan(
        const GlobalState &state, Proposition *goal);
    void mark_relaxed_plan(const GlobalState &state, Proposition *goal); // Peter: Needed for M&S for finding label sets
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &state);
    void compute_relaxed_plan(const GlobalState &state, RelaxedPlan &returned_relaxed_plan); // Peter: Needed for M&S for finding label set
public:
    FFHeuristic(const options::Options &options);
    ~FFHeuristic();
};

}

#endif
