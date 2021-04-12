#pragma once

#include "abstract_operator.h"
#include "abstract_state.h"
#include "engine_interfaces.h"
#include "qualitative_result_store.h"
#include "quantitative_result_store.h"
#include "types.h"

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

class ProbabilisticProjection {
public:
    ProbabilisticProjection(
        const std::vector<int>& variables,
        const std::vector<int>& domains);
    ~ProbabilisticProjection() = default;

    std::shared_ptr<AbstractStateMapper> get_abstract_state_mapper() const;
    QualitativeResultStore& get_abstract_goal_states();

    unsigned int num_states() const;

    // Returns the pattern (i.e. all variables used) of the PDB
    const Pattern &get_pattern() const {
        return state_mapper_->get_variables();
    }

protected:
    void setup_abstract_goal();
    void setup_abstract_operators() const;
    void prepare_regression();

    std::vector<int> var_index_;
    std::shared_ptr<AbstractStateMapper> state_mapper_;
    std::vector<std::pair<int, int>> projected_goal_;
    mutable std::vector<AbstractOperator> abstract_operators_;
    AbstractState initial_state_;
    QualitativeResultStore goal_states_;

    mutable
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const AbstractOperator*>>
        progression_aops_generator_;
    mutable
    std::shared_ptr<successor_generator::SuccessorGenerator<AbstractState>>
        regression_aops_generator_;
};

} // namespace pdbs
} // namespace probabilistic
