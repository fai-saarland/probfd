#pragma once

#include "../probabilistic_projection.h"

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
        QualitativeResultStore* dead_ends = nullptr);

    void dump_graphviz(
        const std::string& path,
        bool transition_labels = true,
        bool values = true);

private:
    void dump_graphviz_no_values(
        const std::string& path,
        bool transition_labels);

    void dump_graphviz_with_values(
        MaxProbAbstractAnalysisResult* values,
        value_type::value_t v0,
        value_type::value_t v1,
        const std::string& path,
        bool transition_labels);
};

} // namespace pdbs
} // namespace probabilistic
