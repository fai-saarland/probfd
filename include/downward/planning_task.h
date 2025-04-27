#ifndef PLANNING_TASK_H
#define PLANNING_TASK_H

#include "downward/tuple_utils.h"

#include <memory>
#include <tuple>

namespace downward {
class VariableSpace;
class AxiomSpace;
class OperatorSpace;
class GoalFactList;
class InitialStateValues;
} // namespace downward

namespace downward {

using PlanningTaskTuple = std::tuple<
    VariableSpace&,
    AxiomSpace&,
    OperatorSpace&,
    GoalFactList&,
    InitialStateValues&>;

using UniquePlanningTask = std::tuple<
    std::unique_ptr<VariableSpace>,
    std::unique_ptr<AxiomSpace>,
    std::unique_ptr<OperatorSpace>,
    std::unique_ptr<GoalFactList>,
    std::unique_ptr<InitialStateValues>>;

using SharedPlanningTask = std::tuple<
    std::shared_ptr<VariableSpace>,
    std::shared_ptr<AxiomSpace>,
    std::shared_ptr<OperatorSpace>,
    std::shared_ptr<GoalFactList>,
    std::shared_ptr<InitialStateValues>>;

inline const VariableSpace& get_variables(const PlanningTaskTuple& task)
{
    return get<VariableSpace&>(task);
}

inline VariableSpace& get_variables(PlanningTaskTuple& task)
{
    return get<VariableSpace&>(task);
}

inline const VariableSpace& get_variables(const UniquePlanningTask& task)
{
    return *get_unique<VariableSpace>(task);
}

inline const VariableSpace& get_variables(const SharedPlanningTask& task)
{
    return *get_shared<VariableSpace>(task);
}

inline const std::shared_ptr<VariableSpace>&
get_shared_variables(const SharedPlanningTask& task)
{
    return get_shared<VariableSpace>(task);
}

inline const AxiomSpace& get_axioms(const PlanningTaskTuple& task)
{
    return get<AxiomSpace&>(task);
}

inline AxiomSpace& get_axioms(PlanningTaskTuple& task)
{
    return get<AxiomSpace&>(task);
}

inline const AxiomSpace& get_axioms(const UniquePlanningTask& task)
{
    return *get_unique<AxiomSpace>(task);
}

inline const AxiomSpace& get_axioms(const SharedPlanningTask& task)
{
    return *get_shared<AxiomSpace>(task);
}

inline const std::shared_ptr<AxiomSpace>&
get_shared_axioms(const SharedPlanningTask& task)
{
    return get_shared<AxiomSpace>(task);
}

inline const OperatorSpace& get_operators(const PlanningTaskTuple& task)
{
    return get<OperatorSpace&>(task);
}

inline OperatorSpace& get_operators(PlanningTaskTuple& task)
{
    return get<OperatorSpace&>(task);
}

inline const OperatorSpace& get_operators(const UniquePlanningTask& task)
{
    return *get_unique<OperatorSpace>(task);
}

inline const OperatorSpace& get_operators(const SharedPlanningTask& task)
{
    return *get_shared<OperatorSpace>(task);
}

inline const std::shared_ptr<OperatorSpace>&
get_shared_operators(const SharedPlanningTask& task)
{
    return get_shared<OperatorSpace>(task);
}

inline const InitialStateValues& get_init(const PlanningTaskTuple& task)
{
    return get<InitialStateValues&>(task);
}

inline InitialStateValues& get_init(PlanningTaskTuple& task)
{
    return get<InitialStateValues&>(task);
}

inline const InitialStateValues& get_init(const UniquePlanningTask& task)
{
    return *get_unique<InitialStateValues>(task);
}

inline const InitialStateValues& get_init(const SharedPlanningTask& task)
{
    return *get_shared<InitialStateValues>(task);
}

inline const std::shared_ptr<InitialStateValues>&
get_shared_init(const SharedPlanningTask& task)
{
    return get_shared<InitialStateValues>(task);
}

inline const GoalFactList& get_goal(const PlanningTaskTuple& task)
{
    return get<GoalFactList&>(task);
}

inline GoalFactList& get_goal(PlanningTaskTuple& task)
{
    return get<GoalFactList&>(task);
}

inline const GoalFactList& get_goal(const UniquePlanningTask& task)
{
    return *get_unique<GoalFactList>(task);
}

inline const GoalFactList& get_goal(const SharedPlanningTask& task)
{
    return *get_shared<GoalFactList>(task);
}

inline const std::shared_ptr<GoalFactList>&
get_shared_goal(const SharedPlanningTask& task)
{
    return get_shared<GoalFactList>(task);
}

} // namespace downward

#endif
