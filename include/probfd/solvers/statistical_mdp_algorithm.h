#ifndef PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H
#define PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H

#include "probfd/solvers/statistical_mdp_algorithm_fwd.h"

#include "probfd/aliases.h"
#include "probfd/fdr_types.h"

#include <iosfwd>
#include <memory>

// Forward Declarations
class State;
class OperatorID;

namespace probfd {
class ProgressReport;

class ProbabilisticTask;
class TaskHeuristicFactory;
class TaskStateSpaceFactory;

template <typename, typename>
class MDP;

template <typename, typename>
class Policy;

template <typename>
class Heuristic;
} // namespace probfd

namespace probfd::solvers {

template <typename State = State, typename Action = OperatorID>
class StatisticalMDPAlgorithm {
protected:
    using PolicyType = Policy<State, Action>;
    using MDPType = MDP<State, Action>;
    using HeuristicType = Heuristic<State>;

public:
    virtual ~StatisticalMDPAlgorithm() = default;

    /**
     * @brief Computes a partial policy for the input state.
     */
    virtual std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

template <typename State, typename Action>
class StatisticalMDPAlgorithmFactory {
public:
    using AlgorithmType = StatisticalMDPAlgorithm<State, Action>;

    virtual ~StatisticalMDPAlgorithmFactory() = default;

    /**
     * @brief Factory method a new instance of the encapsulated MDP algorithm.
     */
    virtual std::unique_ptr<AlgorithmType>
    create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) = 0;

    /**
     * @brief Returns the name of the MDP algorithm returned by the factory.
     */
    virtual std::string get_algorithm_name() const = 0;
};
} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H
