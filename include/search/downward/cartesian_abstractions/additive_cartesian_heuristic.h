#ifndef CEGAR_ADDITIVE_CARTESIAN_HEURISTIC_H
#define CEGAR_ADDITIVE_CARTESIAN_HEURISTIC_H

#include "downward/heuristic.h"

#include <vector>

namespace cartesian_abstractions {
class CartesianHeuristicFunction;

/*
  Store CartesianHeuristicFunctions and compute overall heuristic by
  summing all of their values.
*/
class AdditiveCartesianHeuristic : public Heuristic {
    const std::vector<CartesianHeuristicFunction> heuristic_functions;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    explicit AdditiveCartesianHeuristic(const plugins::Options& opts);
};
} // namespace cartesian_abstractions

#endif
