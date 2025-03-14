#ifndef PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_FWD_H
#define PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_FWD_H

// Forward Declarations
class State;
class OperatorID;

namespace probfd::solvers {

template <typename State, typename Action>
class StatisticalMDPAlgorithm;

template <typename State, typename Action>
class StatisticalMDPAlgorithmFactory;

using FDRStatisticalMDPAlgorithm = StatisticalMDPAlgorithm<State, OperatorID>;
using FDRStatisticalMDPAlgorithmFactory =
    StatisticalMDPAlgorithmFactory<State, OperatorID>;

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_FWD_H
