#ifndef LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_H
#define LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_H

#include "downward/task_dependent_factory_fwd.h"

#include "downward/landmarks/landmark_factory.h"

namespace downward {
class MutexInformation;
class EffectsProxy;
} // namespace downward

namespace downward::landmarks {
class LandmarkFactoryReasonableOrdersHPS : public LandmarkFactory {
    std::shared_ptr<LandmarkFactory> lm_factory;
    std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory;

    void generate_landmarks(const SharedAbstractTask& task) override;

    void approximate_reasonable_orders(
        const VariableSpace& variables,
        const AxiomSpace& axioms,
        const ClassicalOperatorSpace& operators,
        const MutexInformation& mutexes);

    bool interferes(
        const VariableSpace& variables,
        const AxiomSpace& axioms,
        const ClassicalOperatorSpace& operators,
        const MutexInformation& mutexes,
        const Landmark& landmark_a,
        const Landmark& landmark_b) const;

    void collect_ancestors(
        std::unordered_set<LandmarkNode*>& result,
        LandmarkNode& node);

    bool effect_always_happens(
        const VariableSpace& variables,
        const EffectsProxy& effects,
        std::set<FactPair>& eff) const;

public:
    LandmarkFactoryReasonableOrdersHPS(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
        utils::Verbosity verbosity);

    bool supports_conditional_effects() const override;
};
} // namespace downward::landmarks

#endif
