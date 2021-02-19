#pragma once

#include "probabilistic_projection.h"

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

struct ExpCostAbstractAnalysisResult {
    QuantitativeResultStore* value_table = nullptr;
    unsigned reachable_states = 0;
};

class ExpCostProjection : public ProbabilisticProjection {
public:
    using ProbabilisticProjection::ProbabilisticProjection;

    ~ExpCostProjection() = default;

    ExpCostAbstractAnalysisResult compute_value_table(
        AbstractStateEvaluator* state_reward,
        AbstractOperatorEvaluator* transition_reward,
        value_type::value_t dead_end_value,
        value_type::value_t upper);
};

extern ExpCostAbstractAnalysisResult compute_value_table(
    ExpCostProjection& projection);

} // namespace pdbs
} // namespace probabilistic
