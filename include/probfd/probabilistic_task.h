#ifndef PROBFD_PROBABILISTIC_TASK_H
#define PROBFD_PROBABILISTIC_TASK_H

#include "probfd/probabilistic_operator_cost_function_fwd.h"

#include "downward/abstract_task.h"

namespace probfd {
class ProbabilisticOperatorSpace;
class TerminationCosts;
} // namespace probfd

namespace probfd {

using ProbabilisticTaskTuple = std::tuple<
    downward::VariableSpace&,
    downward::AxiomSpace&,
    ProbabilisticOperatorSpace&,
    downward::GoalFactList&,
    downward::InitialStateValues&,
    ProbabilisticOperatorCostFunction&,
    TerminationCosts&>;

using UniqueProbabilisticTask = std::tuple<
    std::unique_ptr<downward::VariableSpace>,
    std::unique_ptr<downward::AxiomSpace>,
    std::unique_ptr<ProbabilisticOperatorSpace>,
    std::unique_ptr<downward::GoalFactList>,
    std::unique_ptr<downward::InitialStateValues>,
    std::unique_ptr<ProbabilisticOperatorCostFunction>,
    std::unique_ptr<TerminationCosts>>;

using SharedProbabilisticTask = std::tuple<
    std::shared_ptr<downward::VariableSpace>,
    std::shared_ptr<downward::AxiomSpace>,
    std::shared_ptr<ProbabilisticOperatorSpace>,
    std::shared_ptr<downward::GoalFactList>,
    std::shared_ptr<downward::InitialStateValues>,
    std::shared_ptr<ProbabilisticOperatorCostFunction>,
    std::shared_ptr<TerminationCosts>>;

inline const downward::VariableSpace&
get_variables(const ProbabilisticTaskTuple& task)
{
    return get<downward::VariableSpace&>(task);
}

inline downward::VariableSpace& get_variables(ProbabilisticTaskTuple& task)
{
    return get<downward::VariableSpace&>(task);
}

inline const downward::VariableSpace&
get_variables(const UniqueProbabilisticTask& task)
{
    return *get_unique<downward::VariableSpace>(task);
}

inline const downward::VariableSpace&
get_variables(const SharedProbabilisticTask& task)
{
    return *get_shared<downward::VariableSpace>(task);
}

inline const std::shared_ptr<downward::VariableSpace>&
get_shared_variables(const SharedProbabilisticTask& task)
{
    return get_shared<downward::VariableSpace>(task);
}

inline const downward::AxiomSpace&
get_axioms(const ProbabilisticTaskTuple& task)
{
    return get<downward::AxiomSpace&>(task);
}

inline downward::AxiomSpace& get_axioms(ProbabilisticTaskTuple& task)
{
    return get<downward::AxiomSpace&>(task);
}

inline const downward::AxiomSpace&
get_axioms(const UniqueProbabilisticTask& task)
{
    return *get_unique<downward::AxiomSpace>(task);
}

inline const downward::AxiomSpace&
get_axioms(const SharedProbabilisticTask& task)
{
    return *get_shared<downward::AxiomSpace>(task);
}

inline const std::shared_ptr<downward::AxiomSpace>&
get_shared_axioms(const SharedProbabilisticTask& task)
{
    return get_shared<downward::AxiomSpace>(task);
}

inline const ProbabilisticOperatorSpace&
get_operators(const ProbabilisticTaskTuple& task)
{
    return get<ProbabilisticOperatorSpace&>(task);
}

inline ProbabilisticOperatorSpace& get_operators(ProbabilisticTaskTuple& task)
{
    return get<ProbabilisticOperatorSpace&>(task);
}

inline const ProbabilisticOperatorSpace&
get_operators(const UniqueProbabilisticTask& task)
{
    return *get_unique<ProbabilisticOperatorSpace>(task);
}

inline const ProbabilisticOperatorSpace&
get_operators(const SharedProbabilisticTask& task)
{
    return *get_shared<ProbabilisticOperatorSpace>(task);
}

inline const std::shared_ptr<ProbabilisticOperatorSpace>&
get_shared_operators(const SharedProbabilisticTask& task)
{
    return get_shared<ProbabilisticOperatorSpace>(task);
}

inline const downward::InitialStateValues&
get_init(const ProbabilisticTaskTuple& task)
{
    return get<downward::InitialStateValues&>(task);
}

inline downward::InitialStateValues& get_init(ProbabilisticTaskTuple& task)
{
    return get<downward::InitialStateValues&>(task);
}

inline const downward::InitialStateValues&
get_init(const UniqueProbabilisticTask& task)
{
    return *get_unique<downward::InitialStateValues>(task);
}

inline const downward::InitialStateValues&
get_init(const SharedProbabilisticTask& task)
{
    return *get_shared<downward::InitialStateValues>(task);
}

inline const std::shared_ptr<downward::InitialStateValues>&
get_shared_init(const SharedProbabilisticTask& task)
{
    return get_shared<downward::InitialStateValues>(task);
}

inline const downward::GoalFactList&
get_goal(const ProbabilisticTaskTuple& task)
{
    return get<downward::GoalFactList&>(task);
}

inline downward::GoalFactList& get_goal(ProbabilisticTaskTuple& task)
{
    return get<downward::GoalFactList&>(task);
}

inline const downward::GoalFactList&
get_goal(const UniqueProbabilisticTask& task)
{
    return *get_unique<downward::GoalFactList>(task);
}

inline const downward::GoalFactList&
get_goal(const SharedProbabilisticTask& task)
{
    return *get_shared<downward::GoalFactList>(task);
}

inline const std::shared_ptr<downward::GoalFactList>&
get_shared_goal(const SharedProbabilisticTask& task)
{
    return get_shared<downward::GoalFactList>(task);
}

inline const ProbabilisticOperatorCostFunction&
get_cost_function(const ProbabilisticTaskTuple& task)
{
    return get<ProbabilisticOperatorCostFunction&>(task);
}

inline ProbabilisticOperatorCostFunction&
get_cost_function(ProbabilisticTaskTuple& task)
{
    return get<ProbabilisticOperatorCostFunction&>(task);
}

inline const ProbabilisticOperatorCostFunction&
get_cost_function(const UniqueProbabilisticTask& task)
{
    return *get_unique<ProbabilisticOperatorCostFunction>(task);
}

inline const ProbabilisticOperatorCostFunction&
get_cost_function(const SharedProbabilisticTask& task)
{
    return *get_shared<ProbabilisticOperatorCostFunction>(task);
}

inline const std::shared_ptr<ProbabilisticOperatorCostFunction>&
get_shared_cost_function(const SharedProbabilisticTask& task)
{
    return get_shared<ProbabilisticOperatorCostFunction>(task);
}

inline const TerminationCosts&
get_termination_costs(const ProbabilisticTaskTuple& task)
{
    return get<TerminationCosts&>(task);
}

inline TerminationCosts& get_termination_costs(ProbabilisticTaskTuple& task)
{
    return get<TerminationCosts&>(task);
}

inline const TerminationCosts&
get_termination_costs(const UniqueProbabilisticTask& task)
{
    return *get_unique<TerminationCosts>(task);
}

inline const TerminationCosts&
get_termination_costs(const SharedProbabilisticTask& task)
{
    return *get_shared<TerminationCosts>(task);
}

inline const std::shared_ptr<TerminationCosts>&
get_shared_termination_costs(const SharedProbabilisticTask& task)
{
    return get_shared<TerminationCosts>(task);
}

} // namespace probfd

#endif
