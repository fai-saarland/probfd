#pragma once

#include "abstract_operator.h"
#include "abstract_state.h"
#include "engine_interfaces.h"
#include "qualitative_result_store.h"
#include "quantitative_result_store.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class AnalysisObjective;

namespace pdbs {

struct AbstractAnalysisResult {
    QuantitativeResultStore* value_table = nullptr;
    QualitativeResultStore* dead_ends = nullptr;
    QualitativeResultStore* one_states = nullptr;

    value_type::value_t one_state_reward = value_type::zero;

    unsigned reachable_states = 0;
    unsigned dead = 0;
    unsigned one = 0;
};

class ProbabilisticProjection {
public:
    ProbabilisticProjection(
        const std::vector<int>& variables,
        const std::vector<int>& domains);
    ~ProbabilisticProjection() = default;

    std::shared_ptr<AbstractStateMapper> get_abstract_state_mapper() const;
    QualitativeResultStore& get_abstract_goal_states();

    QualitativeResultStore compute_dead_ends();

    AbstractAnalysisResult compute_value_table(
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
        AbstractAnalysisResult* values,
        value_type::value_t v0,
        value_type::value_t v1,
        const std::string& path,
        bool transition_labels);

private:
    void setup_abstract_goal();
    void setup_abstract_operators();
    void prepare_regression();

    std::vector<int> var_index_;
    std::shared_ptr<AbstractStateMapper> state_mapper_;
    std::vector<std::pair<int, int>> projected_goal_;
    std::vector<AbstractOperator> abstract_operators_;
    AbstractState initial_state_;
    QualitativeResultStore goal_states_;

    std::shared_ptr<
        successor_generator::SuccessorGenerator<const AbstractOperator*>>
        progression_aops_generator_;
    std::shared_ptr<successor_generator::SuccessorGenerator<AbstractState>>
        regression_aops_generator_;
};

extern AbstractAnalysisResult compute_value_table(
    ProbabilisticProjection* projection,
    AnalysisObjective* objective,
    QualitativeResultStore* dead_ends = nullptr);

extern void dump_graphviz(
    ProbabilisticProjection* projection,
    AnalysisObjective* objective,
    const std::string& path,
    bool transition_labels,
    bool values);

} // namespace pdbs
} // namespace probabilistic
