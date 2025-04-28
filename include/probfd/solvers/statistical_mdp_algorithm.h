#ifndef PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H
#define PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H

#include "probfd/aliases.h"
#include "probfd/fdr_types.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_heuristic_factory_fwd.h"
#include "probfd/task_state_space_factory_fwd.h"

#include <iosfwd>
#include <memory>

// Forward Declarations
namespace downward {
class State;
class OperatorID;
} // namespace downward

namespace probfd {
class ProgressReport;

template <typename, typename>
class MDP;

template <typename, typename>
class Policy;

template <typename>
class Heuristic;
} // namespace probfd

namespace probfd::solvers {

class StatisticalMDPAlgorithm {
protected:
    using PolicyType = Policy<downward::State, downward::OperatorID>;
    using MDPType = MDP<downward::State, downward::OperatorID>;
    using HeuristicType = Heuristic<downward::State>;

public:
    virtual ~StatisticalMDPAlgorithm() = default;

    /**
     * @brief Computes a partial policy for the input state.
     */
    virtual std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<downward::State> state,
        ProgressReport progress,
        double max_time) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

class StatisticalMDPAlgorithmFactory {
public:
    virtual ~StatisticalMDPAlgorithmFactory() = default;

    /**
     * @brief Factory method a new instance of the encapsulated MDP algorithm.
     */
    virtual std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) = 0;

    /**
     * @brief Returns the name of the MDP algorithm returned by the factory.
     */
    virtual std::string get_algorithm_name() const = 0;
};

class AlgorithmAdaptor : public StatisticalMDPAlgorithm {
    std::unique_ptr<FDRMDPAlgorithm> algorithm;

public:
    explicit AlgorithmAdaptor(std::unique_ptr<FDRMDPAlgorithm> algorithm);

    ~AlgorithmAdaptor() override;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<downward::State> state,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream&) const override;
};

} // namespace probfd::solvers

#endif // PROBFD_SOLVERS_STATISTICAL_MDP_ALGORITHM_H
