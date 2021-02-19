#pragma once

#include "probabilistic_projection.h"

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

struct MaxProbAbstractAnalysisResult {
    QuantitativeResultStore* value_table = nullptr;
    QualitativeResultStore* dead_ends = nullptr;
    QualitativeResultStore* one_states = nullptr;

    value_type::value_t one_state_reward = value_type::zero;

    unsigned reachable_states = 0;
    unsigned dead = 0;
    unsigned one = 0;
};

class MaxProbProjection : public ProbabilisticProjection {
public:
    using ProbabilisticProjection::ProbabilisticProjection;

    ~MaxProbProjection() = default;

    QualitativeResultStore compute_dead_ends();

    MaxProbAbstractAnalysisResult compute_value_table(
        AbstractStateEvaluator* state_reward,
        AbstractOperatorEvaluator* transition_reward,
        value_type::value_t dead_end_value,
        value_type::value_t upper,
        bool one_states,
        value_type::value_t one_state_reward,
        QualitativeResultStore* dead_ends = nullptr);

    void dump_graphviz(
        AbstractStateEvaluator* state_reward,
        const std::string& path,
        bool transition_labels);

    void dump_graphviz(
        AbstractStateEvaluator* state_reward,
        MaxProbAbstractAnalysisResult* values,
        value_type::value_t v0,
        value_type::value_t v1,
        const std::string& path,
        bool transition_labels);
};

extern MaxProbAbstractAnalysisResult compute_value_table(
    MaxProbProjection& projection,
    QualitativeResultStore* dead_ends = nullptr);

extern void dump_graphviz(
    MaxProbProjection& projection,
    const std::string& path,
    bool transition_labels,
    bool values);

} // namespace pdbs
} // namespace probabilistic
