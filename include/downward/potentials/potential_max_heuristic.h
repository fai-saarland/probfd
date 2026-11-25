#ifndef POTENTIALS_POTENTIAL_MAX_HEURISTIC_H
#define POTENTIALS_POTENTIAL_MAX_HEURISTIC_H

#include "downward/heuristic.h"

#include <vector>

namespace downward::potentials {
class FactPotentialFunction;
}

namespace downward::potentials {

/*
  Maximize over multiple potential functions.
*/
class PotentialMaxHeuristic : public Heuristic {
    std::vector<FactPotentialFunction> functions;

public:
    PotentialMaxHeuristic(
        std::vector<FactPotentialFunction>&& functions,
        SharedAbstractTask original_task,
        const std::shared_ptr<TaskTransformation>& transform,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    PotentialMaxHeuristic(
        std::vector<FactPotentialFunction>&& functions,
        SharedAbstractTask original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    PotentialMaxHeuristic(
        std::vector<FactPotentialFunction>&& functions,
        SharedAbstractTask original_task,
        SharedAbstractTask transformed_task,
        std::shared_ptr<StateMapping> state_mapping,
        std::shared_ptr<InverseOperatorMapping> inv_operator_mapping,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~PotentialMaxHeuristic() override;
    
protected:
    int compute_heuristic(const State& ancestor_state) override;
};

} // namespace potentials

#endif
