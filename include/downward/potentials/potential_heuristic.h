#ifndef POTENTIALS_POTENTIAL_HEURISTIC_H
#define POTENTIALS_POTENTIAL_HEURISTIC_H

#include "downward/heuristic.h"

#include <memory>

namespace downward::potentials {
class PotentialFunction;

/*
  Use an internal potential function to evaluate a given state.
*/
class PotentialHeuristic : public Heuristic {
    std::unique_ptr<PotentialFunction> function;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    PotentialHeuristic(
        std::unique_ptr<PotentialFunction> function,
        SharedAbstractTask original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    PotentialHeuristic(
        std::unique_ptr<PotentialFunction> function,
        SharedAbstractTask original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    PotentialHeuristic(
        std::unique_ptr<PotentialFunction> function,
        SharedAbstractTask original_task,
        SharedAbstractTask transformed_task,
        std::shared_ptr<StateMapping> state_mapping,
        std::shared_ptr<InverseOperatorMapping> inv_operator_mapping,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~PotentialHeuristic() override;
};
} // namespace potentials

#endif // POTENTIALS_POTENTIAL_HEURISTIC_H
