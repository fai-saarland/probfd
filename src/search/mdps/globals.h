#pragma once

#include "../operator_cost.h"
#include "probabilistic_operator.h"

#include <memory>
#include <vector>

class StateRegistry;

namespace int_packer {
class IntPacker;
}

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace probabilistic {

namespace analysis_objectives {
class AnalysisObjective;
}

void prepare_globals(
    std::shared_ptr<analysis_objectives::AnalysisObjective> objective);
void print_task_info();

extern bool g_probabilistic_mode_enabled;

extern const int g_unlimited_budget;
extern bool g_steps_bounded;
extern int g_step_bound;
extern int g_step_var;
extern OperatorCost g_step_cost_type;

/// The global list of operators of the problem's MDP model.
extern std::vector<ProbabilisticOperator> g_operators;

extern int_packer::IntPacker* g_state_packer;
extern std::vector<int> g_initial_state_values;
extern std::shared_ptr<
    successor_generator::SuccessorGenerator<const ProbabilisticOperator*>>
    g_successor_generator;

extern std::shared_ptr<analysis_objectives::AnalysisObjective>
    g_analysis_objective;

} // namespace probabilistic
