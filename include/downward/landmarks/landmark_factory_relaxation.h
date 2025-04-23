#ifndef LANDMARKS_LANDMARK_FACTORY_RELAXATION_H
#define LANDMARKS_LANDMARK_FACTORY_RELAXATION_H

#include "downward/landmarks/landmark_factory.h"

namespace downward::landmarks {
class Exploration;

class LandmarkFactoryRelaxation : public LandmarkFactory {
protected:
    explicit LandmarkFactoryRelaxation(utils::Verbosity verbosity);

    /* Test whether the relaxed planning task is solvable without
       achieving the excluded landmark. */
    bool relaxed_task_solvable(
        const AbstractTask& task,
        Exploration& exploration,
        const Landmark& exclude,
        bool use_unary_relaxation) const;

private:
    void generate_landmarks(const std::shared_ptr<AbstractTask>& task) override;

    virtual void generate_relaxed_landmarks(
        const std::shared_ptr<AbstractTask>& task,
        Exploration& exploration) = 0;
    void postprocess(const AbstractTask& task, Exploration& exploration);

    void
    calc_achievers(const AbstractTask& task, Exploration& exploration);
};
} // namespace downward::landmarks

#endif
