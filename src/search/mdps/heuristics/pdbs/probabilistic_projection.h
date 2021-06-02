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
protected:
    using ProgressionSuccessorGenerator = 
        successor_generator::SuccessorGenerator<const AbstractOperator*>;

public:
    ProbabilisticProjection(
        const Pattern& pattern,
        const std::vector<int>& domains);
    ~ProbabilisticProjection() = default;

    std::shared_ptr<AbstractStateMapper> get_abstract_state_mapper() const;
    std::unordered_set<AbstractState>& get_abstract_goal_states();

    unsigned int num_states() const;

    // Returns the pattern (i.e. all variables used) of the PDB

    const Pattern &get_pattern() const;
    
private:
    void setup_abstract_goal();
    void prepare_progression();

protected:
    void prepare_regression() const;
    void add_to_goals(AbstractState state);

    std::vector<int> var_index_;
    std::shared_ptr<AbstractStateMapper> state_mapper_;
    std::vector<std::pair<int, int>> sparse_goal_;
    AbstractState initial_state_;
    std::unordered_set<AbstractState> goal_states_;

    std::vector<AbstractOperator> abstract_operators_;
    std::shared_ptr<ProgressionSuccessorGenerator> progression_aops_generator_;
};

} // namespace pdbs
} // namespace probabilistic
