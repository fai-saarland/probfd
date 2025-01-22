#ifndef LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_H
#define LANDMARKS_LANDMARK_FACTORY_REASONABLE_ORDERS_HPS_H

#include "downward/landmarks/landmark_factory.h"

namespace downward {
class MutexFactory;
class MutexInformation;
}

namespace downward::landmarks {
class LandmarkFactoryReasonableOrdersHPS : public LandmarkFactory {
    std::shared_ptr<LandmarkFactory> lm_factory;
    std::shared_ptr<MutexFactory> mutex_factory;

    virtual void
    generate_landmarks(const std::shared_ptr<AbstractTask>& task) override;

    void approximate_reasonable_orders(
        const TaskProxy& task_proxy,
        const MutexInformation& mutexes);
    bool interferes(
        const TaskProxy& task_proxy,
        const MutexInformation& mutexes,
        const Landmark& landmark_a,
        const Landmark& landmark_b) const;
    void collect_ancestors(
        std::unordered_set<LandmarkNode*>& result,
        LandmarkNode& node);
    bool effect_always_happens(
        const VariablesProxy& variables,
        const EffectsProxy& effects,
        std::set<FactPair>& eff) const;

public:
    LandmarkFactoryReasonableOrdersHPS(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        std::shared_ptr<MutexFactory> mutex_factory,
        utils::Verbosity verbosity);

    virtual bool supports_conditional_effects() const override;
};
} // namespace landmarks

#endif
