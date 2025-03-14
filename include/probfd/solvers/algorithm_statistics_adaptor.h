#ifndef PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_H
#define PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_H

#include "probfd/solvers/statistical_mdp_algorithm.h"

namespace probfd::solvers {

template <typename State = State, typename Action = OperatorID>
class AlgorithmAdaptor : public StatisticalMDPAlgorithm<State, Action> {
    using MDPType = AlgorithmAdaptor::StatisticalMDPAlgorithm::MDPType;
    using HeuristicType =
        AlgorithmAdaptor::StatisticalMDPAlgorithm::HeuristicType;
    using PolicyType = AlgorithmAdaptor::StatisticalMDPAlgorithm::PolicyType;

    std::unique_ptr<MDPAlgorithm<State, Action>> algorithm;

public:
    explicit AlgorithmAdaptor(
        std::unique_ptr<MDPAlgorithm<State, Action>> algorithm);

    ~AlgorithmAdaptor() override;

    virtual std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) override;

    virtual void print_statistics(std::ostream&) const override;
};

} // namespace probfd::solvers

#define PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_GUARD
#include "algorithm_statistics_adaptor-impl.h"
#undef PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_GUARD

#endif // PROBFD_SOLVERS_ALGORITHM_STATISTICS_ADAPTOR_H
